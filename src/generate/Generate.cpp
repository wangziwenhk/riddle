#include "Generate.h"

#include <iostream>
#include <llvm/IR/Verifier.h>

#include "nodes.h"
#include "config.h"
#include "OperatorImpl.h"
#include "semantic/Operators.h"

using namespace std;

namespace riddle {
    llvm::Type *Generate::getPrimitiveType(const string &name) {
        static const unordered_map<string, llvm::Type *> mp = {
            {"int", builder.getInt32Ty()},
            {"float", builder.getFloatTy()},
            {"char", builder.getInt8Ty()},
            {"bool", builder.getInt1Ty()},
            {"void", builder.getVoidTy()},
            {"long", builder.getInt64Ty()},
        };

        const auto it = mp.find(name);
        if (it == mp.end()) {
            throw runtime_error("Unknown primitive type: " + name);
        }
        return it->second;
    }

    llvm::Value *cast(llvm::Value *value, llvm::Type *type, ExprNode::CaseType cast_type, llvm::IRBuilder<> &builder) {
        switch (cast_type) {
            case ExprNode::ZExt: {
                return builder.CreateZExt(value, type);
            }
            case ExprNode::SExt: {
                return builder.CreateSExt(value, type);
            }
            case ExprNode::None: {
                return value;
            }
            default: break;
        }
        throw runtime_error("Unknown cast type");
    }

    llvm::Type *Generate::parseType(const shared_ptr<TypeInfo> &type, const size_t depth) { // NOLINT(*-no-recursion)
        if (depth > MAX_DEPTH) {
            throw runtime_error("Recursive depth exceeds limit");
        }
        if (type == nullptr) {
            return nullptr;
        }
        if (type->type) {
            return type->type;
        }
        switch (type->getTypeKind()) {
            case TypeInfo::Primitive: {
                type->type = getPrimitiveType(type->name);
                return type->type;
            }
            case TypeInfo::Struct: {
                return type->type;
            }
            case TypeInfo::Pointer: {
                return llvm::PointerType::get(parseType(type->getPointerTo(), depth + 1), 0);
            }
            default: break;
        }
        return nullptr;
    }

    Generate::Generate(): info(make_unique<BuildInfo>()), context(globalContext.get()), builder(*globalContext) {
        info->module = make_unique<llvm::Module>("main", *globalContext);
    }

    any Generate::visitProgram(ProgramNode *node) {
        for (const auto &i: node->body) {
            visit(i);
        }
        std::string verifyError;
        llvm::raw_string_ostream errorStream(verifyError);
        if (llvm::verifyModule(*info->module, &errorStream)) {
            errorStream.flush();
            cerr << "Module verification failed: " + verifyError;
        }
        info->module->print(llvm::outs(), nullptr);
        return nullptr;
    }

    any Generate::visitInteger(IntegerNode *node) {
        llvm::Value *value = builder.getInt32(node->value);;
        return value;
    }

    any Generate::visitFloat(FloatNode *node) {
        llvm::Value *value = llvm::ConstantFP::get(builder.getFloatTy(), node->value);
        return value;
    }

    std::any Generate::visitChar(CharNode *node) {
        llvm::Value *value = builder.getInt8(node->value);
        return value;
    }

    std::any Generate::visitBoolean(BooleanNode *node) {
        llvm::Value *value = builder.getInt1(node->value);
        return value;
    }

    any Generate::visitFuncDecl(FuncDeclNode *node) {
        // get args
        vector<llvm::Type *> argTypes;
        for (const auto &i: node->args) {
            argTypes.emplace_back(parseType(i->obj->type));
        }

        const auto returnType = parseType(node->obj->returnType);
        const auto funcType = llvm::FunctionType::get(returnType, argTypes, false);
        const auto func = llvm::Function::Create(funcType, llvm::GlobalValue::ExternalLinkage, node->name, info->module.get());

        // reset arg's memory
        unsigned index = 0;
        for (const auto &i: node->args) {
            i->obj->alloca = func->getArg(index);
            ++index;
        }

        node->obj->func = func;

        if (node->body) {
            const auto entry = llvm::BasicBlock::Create(*context, "entry", func);
            builder.SetInsertPoint(entry);
            visit(node->body);
        }
        return nullptr;
    }

    std::any Generate::visitBlock(BlockNode *node) {
        llvm::Value *value = nullptr;
        for (const auto i: node->body) {
            const auto result = visit(i);
            if (result.type() == typeid(nullptr)) {
                continue;
            }
            value = std::any_cast<llvm::Value *>(result);
        }
        return value;
    }

    any Generate::visitVarDecl(VarDeclNode *node) {
        const auto type = parseType(node->obj->type);
        const auto alloca = builder.CreateAlloca(type);
        node->obj->alloca = alloca;
        if (node->value) {
            auto value = any_cast<llvm::Value *>(visit(node->value));
            value = cast(value, type, node->value->cast_type, builder);
            builder.CreateStore(value, alloca);
        }
        return nullptr;
    }

    any Generate::visitObject(ObjectNode *node) {
        switch (node->obj->getKind()) {
            case SemObject::Variable: {
                const auto var = dynamic_pointer_cast<SemVariable>(node->obj);
                llvm::Value *value;
                if (var->isLocalVar) {
                    value = this->builder.CreateLoad(parseType(var->type), var->alloca);
                } else {
                    value = var->alloca;
                }
                return value;
            }
            case SemObject::Function: {
                const auto func = dynamic_pointer_cast<SemFunction>(node->obj);
                llvm::Value *value = func->func;
                return value;
            }
            default:
                break;
        }
        return node->obj;
    }

    any Generate::visitReturn(ReturnNode *node) {
        llvm::Value *value = nullptr;
        if (node->value) {
            const auto result = any_cast<llvm::Value *>(visit(node->value));
            value = builder.CreateRet(result);
        } else {
            value = builder.CreateRetVoid();
        }
        return value;
    }

    any Generate::visitCall(CallNode *node) {
        const auto value = llvm::dyn_cast<llvm::Function>(any_cast<llvm::Value *>(visit(node->value)));
        vector<llvm::Value *> args;
        const auto now_type = value->args().begin();
        for (const auto &i: node->args) {
            auto param = std::any_cast<llvm::Value *>(visit(i));
            param = cast(param, now_type->getType(), i->cast_type, builder);
            args.emplace_back(param);
        }
        llvm::Value *result = builder.CreateCall(value, args);
        return result;
    }

    std::any Generate::visitClassDecl(ClassDeclNode *node) {
        vector<llvm::Type *> types;
        for (const auto &i: node->obj->getStructType()->types) {
            types.emplace_back(parseType(i));
        }
        llvm::StructType *type = llvm::StructType::create(types, node->name);
        node->obj->type->type = type;
        for (const auto &i: node->methods) {
            visit(i);
        }
        return nullptr;
    }

    std::any Generate::visitMemberAccess(MemberAccessNode *node) {
        const auto sty = node->theClass->type->type;
        auto left = std::any_cast<llvm::Value *>(visit(node->left));
        switch (node->type) {
            case MemberAccessNode::Member: {
                if (llvm::isa<llvm::LoadInst>(left)) {
                    const auto ld = llvm::dyn_cast<llvm::LoadInst>(left);
                    left = ld->getPointerOperand();
                }
                const auto index = node->theClass->getMemberIndex(node->right);
                const auto member = builder.CreateStructGEP(sty, left, index);
                const auto type = parseType(std::dynamic_pointer_cast<SemVariable>(node->childObj)->type);
                llvm::Value *result = builder.CreateLoad(type, member);
                return result;
            }
            case MemberAccessNode::Method: {
                llvm::Value *result = std::dynamic_pointer_cast<SemFunction>(node->childObj)->func;
                return result;
            }
            default: throw runtime_error("Unknown MemberAccessNode::Type");
        }
    }

    std::any Generate::visitBinaryOp(BinaryOpNode *node) {
        switch (node->type) {
            case OpNode::Custom: {
                const auto left = std::any_cast<llvm::Value *>(visit(node->left));
                const auto right = std::any_cast<llvm::Value *>(visit(node->right));
                const auto func = node->func->func;
                llvm::Value *result = builder.CreateCall(func, {left, right});
                return result;
            }
            case OpNode::Builtin: {
                const auto left = std::any_cast<llvm::Value *>(visit(node->left));
                const auto right = std::any_cast<llvm::Value *>(visit(node->right));
                const auto func = op::getOpImpl(node->leftType, node->rightType, node->op);
                return func(left, right, builder);
            }
            case OpNode::ShortCircuited: {
                const auto entryBB = builder.GetInsertBlock();
                const auto func = entryBB->getParent();
                const auto rhsBB = llvm::BasicBlock::Create(*context, "bb", func);
                const auto endBB = llvm::BasicBlock::Create(*context, "bb", func);
                const auto left = std::any_cast<llvm::Value *>(visit(node->left));
                builder.CreateCondBr(left, rhsBB, endBB);
                builder.SetInsertPoint(rhsBB);
                const auto right = std::any_cast<llvm::Value *>(visit(node->right));
                builder.CreateBr(endBB);
                builder.SetInsertPoint(endBB);
                auto *phi = builder.CreatePHI(builder.getInt1Ty(), 2);
                if (node->op == "&&") {
                    phi->addIncoming(builder.getFalse(), entryBB);
                } else {
                    phi->addIncoming(builder.getTrue(), entryBB);
                }
                phi->addIncoming(right, rhsBB);
                llvm::Value *result = phi;
                return result;
            }
            default: break;
        }
        return nullptr;
    }

    std::any Generate::visitWhile(WhileNode *node) {
        const auto entryBB = builder.GetInsertBlock();
        const auto func = entryBB->getParent();
        llvm::BasicBlock *condBB = llvm::BasicBlock::Create(*context, "bb", func);
        llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*context, "bb", func);
        llvm::BasicBlock *exitBB = llvm::BasicBlock::Create(*context, "bb", func);

        builder.CreateBr(condBB);
        builder.SetInsertPoint(condBB);
        const auto cond = std::any_cast<llvm::Value *>(visit(node->condition));
        builder.CreateCondBr(cond, loopBB, exitBB);

        builder.SetInsertPoint(loopBB);
        visit(node->body);
        builder.CreateBr(condBB);

        builder.SetInsertPoint(exitBB);
        return nullptr;
    }

    std::any Generate::visitFor(ForNode *node) {
        const auto entryBB = builder.GetInsertBlock();
        const auto func = entryBB->getParent();
        llvm::BasicBlock *condBB = llvm::BasicBlock::Create(*context, "bb", func);
        llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*context, "bb", func);
        llvm::BasicBlock *incBB = llvm::BasicBlock::Create(*context, "bb", func);
        llvm::BasicBlock *exitBB = llvm::BasicBlock::Create(*context, "bb", func);

        visit(node->init);
        builder.CreateBr(condBB);
        builder.SetInsertPoint(condBB);
        const auto cond = std::any_cast<llvm::Value *>(visit(node->condition));
        builder.CreateCondBr(cond, loopBB, exitBB);

        builder.SetInsertPoint(loopBB);
        visit(node->body);
        builder.CreateBr(incBB);

        builder.SetInsertPoint(incBB);
        visit(node->increment);
        builder.CreateBr(condBB);

        builder.SetInsertPoint(exitBB);
        return nullptr;
    }
} // riddle

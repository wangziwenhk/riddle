#pragma once
#include <any>

#include "nodes.h"

namespace riddle {
    class NodeVisitor {
    public:
        virtual ~NodeVisitor() = default;

        virtual std::any visit(ExprNode *node);

        virtual std::any visitProgram(ProgramNode *node);

        virtual std::any visitBlock(BlockNode *node);

        virtual std::any visitArgDecl(ArgDeclNode *node);

        virtual std::any visitFuncDecl(FuncDeclNode *node);

        virtual std::any visitInteger(IntegerNode *node);

        virtual std::any visitFloat(FloatNode *node);

        virtual std::any visitChar(CharNode *node);

        virtual std::any visitBoolean(BooleanNode *node);

        virtual std::any visitObject(ObjectNode *node);

        virtual std::any visitVarDecl(VarDeclNode *node);

        virtual std::any visitReturn(ReturnNode *node);

        virtual std::any visitCall(CallNode *node);

        virtual std::any visitClassDecl(ClassDeclNode *node);

        virtual std::any visitMemberAccess(MemberAccessNode *node);

        virtual std::any visitPointerTo(PointerToNode *node);

        virtual std::any visitBinaryOp(BinaryOpNode *node);

        virtual std::any visitUnaryOp(UnaryOpNode *node);

        virtual std::any visitCompoundOp(CompoundOpNode* node);

        virtual std::any visitIf(IfNode* node);

        virtual std::any visitFor(ForNode* node);

        virtual std::any visitWhile(WhileNode* node);

        virtual std::any visitEnum(EnumNode* node);

        virtual std::any visitUnion(UnionNode* node);
    };
}

/*
 * Copyright (c) 2025 wangziwenhk
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#pragma once
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include "BuildInfo.h"
#include "TypeInfo.h"
#include "nodes/NodeVisitor.h"

namespace riddle {
    class Generate final : public NodeVisitor {
    public:
        std::unique_ptr<BuildInfo> info;

    private:
        llvm::LLVMContext *context;
        llvm::IRBuilder<> builder;

        llvm::Type *getPrimitiveType(const std::string &name);

        /**
         * Parsing a TypeInfo Type to an llvm:: Type* Type is used to generate the LLVM IR
         * @param type TypeInfo to parse
         * @param depth Recursion depth when parsing PointerTypeInfo, preventing infinite recursion
         * @return llvm:: Type* corresponding to this TypeInfo
         */
        llvm::Type *parseType(const std::shared_ptr<TypeInfo> &type, size_t depth = 0);

    public:
        Generate();

        std::any visitProgram(ProgramNode *node) override;

        /**
         * Delete all instructions in a function after the first termination instruction of a BasicBlock
         */
        static void clearPostTerminator(llvm::Function *func);

        std::any visitInteger(IntegerNode *node) override;

        std::any visitFloat(FloatNode *node) override;

        std::any visitChar(CharNode *node) override;

        std::any visitBoolean(BooleanNode *node) override;

        std::any visitString(StringNode *node) override;

        std::any visitFuncDecl(FuncDeclNode *node) override;

        std::any visitBlock(BlockNode *node) override;

        std::any visitVarDecl(VarDeclNode *node) override;

        std::any visitObject(ObjectNode *node) override;

        std::any visitReturn(ReturnNode *node) override;

        std::any visitCall(CallNode *node) override;

        std::any visitClassDecl(ClassDeclNode *node) override;

        std::any visitMemberAccess(MemberAccessNode *node) override;

        llvm::Value *handleMemberAccess(llvm::Value *left, const MemberAccessNode *node, llvm::Type *sty);

        static llvm::Value *handleMethodAccess(const MemberAccessNode *node);

        llvm::Value *handleUnionMemberAccess(llvm::Value *left, const MemberAccessNode *node, llvm::Type *sty);

        std::any visitBinaryOp(BinaryOpNode *node) override;

        std::any visitCompoundOp(CompoundOpNode *node) override;

        std::any visitIf(IfNode *node) override;

        std::any visitWhile(WhileNode *node) override;

        std::any visitFor(ForNode *node) override;

        std::any visitUnion(UnionNode *node) override;
    };
} // riddle

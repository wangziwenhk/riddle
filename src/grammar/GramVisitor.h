#pragma once

#include <any>

#include "nodes.h"
#include "parser/RiddleParserBaseVisitor.h"

namespace riddle {
    class GramVisitor final : public RiddleParserBaseVisitor {
        ProgramNode* program = new ProgramNode;
    public:
        ExprNode *nodeVisit(antlr4::tree::ParseTree *context);

        std::any visitExpressionEnd(RiddleParser::ExpressionEndContext *context) override;

        std::any visitProgram(RiddleParser::ProgramContext *context) override;

        std::any visitModifierList(RiddleParser::ModifierListContext *context) override;

        std::any visitFuncDecl(RiddleParser::FuncDeclContext *context) override;

        std::any visitBlock(RiddleParser::BlockContext *context) override;

        std::any visitInteger(RiddleParser::IntegerContext *context) override;

        std::any visitFloat(RiddleParser::FloatContext *context) override;

        std::any visitChar(RiddleParser::CharContext *context) override;

        std::any visitBoolean(RiddleParser::BooleanContext *context) override;

        std::any visitObject(RiddleParser::ObjectContext *context) override;

        std::any visitVarDecl(RiddleParser::VarDeclContext *context) override;

        std::any visitDeclArgs(RiddleParser::DeclArgsContext *context) override;

        std::any visitReturnStmt(RiddleParser::ReturnStmtContext *context) override;

        std::any visitCallExpr(RiddleParser::CallExprContext *context) override;

        std::any visitClassDecl(RiddleParser::ClassDeclContext *context) override;

        std::any visitMemberAccess(RiddleParser::MemberAccessContext *context) override;

        std::any visitPointerTo(RiddleParser::PointerToContext *context) override;

        std::any visitParenExpr(RiddleParser::ParenExprContext *context) override;

        std::any visitAddOp(RiddleParser::AddOpContext *context) override;

        std::any visitLogicAnd(RiddleParser::LogicAndContext *context) override;

        std::any visitLogicOr(RiddleParser::LogicOrContext *context) override;

        std::any visitEqOp(RiddleParser::EqOpContext *context) override;

        std::any visitBitAnd(RiddleParser::BitAndContext *context) override;

        std::any visitBitOr(RiddleParser::BitOrContext *context) override;

        std::any visitBitXor(RiddleParser::BitXorContext *context) override;

        std::any visitUnaryOp(RiddleParser::UnaryOpContext *context) override;

        std::any visitCompoundAssignOp(RiddleParser::CompoundAssignOpContext *context) override;

        std::any visitIfStmt(RiddleParser::IfStmtContext *context) override;

        std::any visitForStmt(RiddleParser::ForStmtContext *context) override;

        std::any visitWhileStmt(RiddleParser::WhileStmtContext *context) override;

        std::any visitEnumValue(RiddleParser::EnumValueContext *context) override;

        std::any visitEnumStmt(RiddleParser::EnumStmtContext *context) override;
    };
} // riddle

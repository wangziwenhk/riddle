#pragma once

#include <any>

#include "nodes.h"
#include "parser/RiddleParserBaseVisitor.h"

namespace riddle {
    class GramVisitor final : public RiddleParserBaseVisitor {
    public:
        std::shared_ptr<ExprNode> nodeVisit(antlr4::tree::ParseTree *context);

        std::any visitExpressionEnd(RiddleParser::ExpressionEndContext *context) override;

        std::any visitProgram(RiddleParser::ProgramContext *context) override;

        std::any visitFuncDecl(RiddleParser::FuncDeclContext *context) override;

        std::any visitBlock(RiddleParser::BlockContext *context) override;

        std::any visitInteger(RiddleParser::IntegerContext *context) override;

        std::any visitFloat(RiddleParser::FloatContext *context) override;

        std::any visitChar(RiddleParser::CharContext *context) override;

        std::any visitObject(RiddleParser::ObjectContext *context) override;

        std::any visitVarDecl(RiddleParser::VarDeclContext *context) override;

        std::any visitDeclArgs(RiddleParser::DeclArgsContext *context) override;

        std::any visitReturnStmt(RiddleParser::ReturnStmtContext *context) override;

        std::any visitCallExpr(RiddleParser::CallExprContext *context) override;

        std::any visitClassDecl(RiddleParser::ClassDeclContext *context) override;

        std::any visitMemberAccess(RiddleParser::MemberAccessContext *context) override;

        std::any visitPointerTo(RiddleParser::PointerToContext *context) override;
    };
} // riddle

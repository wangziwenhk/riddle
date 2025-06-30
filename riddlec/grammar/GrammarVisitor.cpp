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

#include "GrammarVisitor.h"
#include "ir/IRNode.h"

namespace riddle {
    std::any GrammarVisitor::visitProgram(RiddleParser::ProgramContext *context) {
        for (const auto i:context->children) {
            visit(i);
        }
        return {};
    }

    std::any GrammarVisitor::visitFuncDecl(RiddleParser::FuncDeclContext *context) {
        const auto name = context->getText();
        return RiddleParserBaseVisitor::visitFuncDecl(context);
    }
} // riddle
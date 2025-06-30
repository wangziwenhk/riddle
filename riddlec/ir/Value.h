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
#include <memory>
#include <vector>

#include "RiddleContext.h"
#include "Value.h"

namespace riddle {
    class Use;
    class Instruction;

    class Value {
        friend Use;
        friend Instruction;

    protected:
        RiddleContext &_ctx;

        std::vector<Use *> uses;

        explicit Value(RiddleContext &context): _ctx(context) {}

    public:
        virtual ~Value() {
            _ctx.deallocate(this);
        }

        void replace(Value *V) const;
    };

    class Use {
    public:
        Value *val;
        Instruction *user;
        unsigned opIdx;

        Use(Value *V, Instruction *U, const unsigned index)
            : val(V), user(U), opIdx(index) { V->uses.push_back(this); }
    };

    class UndefValue final : public Value {
    public:
        explicit UndefValue(RiddleContext &context): Value(context) {}
    };

    class Instruction : public Value {
    protected:
        std::vector<Value *> operands;
        std::vector<std::unique_ptr<Use>> opUses;

        explicit Instruction(RiddleContext &context): Value(context) {}

    public:
        void deleteOperand(unsigned index);

        void setOperand(unsigned index, Value *V);
    };
}

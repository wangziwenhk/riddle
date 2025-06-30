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

#include "Value.h"

namespace riddle {
    void Value::replace(Value *V) const {
        for (const auto U: this->uses) {
            if (auto* I = U->user) {
                const unsigned idx = U->opIdx;
                I->setOperand(idx, V);
            }
        }
    }

    void Instruction::deleteOperand(const unsigned index) {
        if (index >= operands.size()) return;

        if (operands[index]) {
            auto &vUses = operands[index]->uses;
            std::erase(vUses, opUses[index].get());
        }

        operands[index] = nullptr;
        opUses[index].reset();
    }

    void Instruction::setOperand(const unsigned index, Value *V) {
        if (index >= operands.size())return;
        const auto oldValue = operands[index];
        if (oldValue == V)return;
        if (oldValue) {
            auto &uses = oldValue->uses;
            std::erase(uses, opUses[index].get());
        }
        operands[index] = V;

        if (!V) {
            opUses[index].reset();
            return;
        }

        if (opUses[index]) {
            opUses[index]->val = V;
        } else {
            opUses[index] = std::make_unique<Use>(V, this, index);
        }
        V->uses.push_back(opUses[index].get());
    }
}

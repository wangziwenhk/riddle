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

#include <string>
#include <unordered_map>
#include <functional>
#include <stdexcept>

#include "support/MemoryPool.h"

namespace riddle {
    class Type;
    class ArrayType;

    class RiddleContext {
        friend Type;
        MemoryPool pool;
        std::unordered_map<std::string, Type *> typeTable;
        std::unordered_map<size_t, Type *> commonTypeTable;

        using Deleter = std::function<void(void*, MemoryPool&)>;
        std::unordered_map<size_t, Deleter> deleterTable;

    public:
        RiddleContext(): pool(1024) {}

        ~RiddleContext();

        template<typename Tp, typename... Args>
        Tp *create(Args &&... args) {
            void *memory = pool.allocate(sizeof(Tp));
            Tp *object = new(memory) Tp(std::forward<Args>(args)...);

            const size_t id = typeid(Tp).hash_code();
            if (!deleterTable.contains(id)) {
                deleterTable[id] = [](void* p, MemoryPool& pool) {
                    static_cast<Tp*>(p)->~Tp();
                    pool.deallocate(p);
                };
            }

            return object;
        }

        template<std::derived_from<Type> Tp, typename... Args>
        Tp *createCommonType(Args &&... args) {
            const size_t index = typeid(Tp).hash_code();
            if (commonTypeTable.contains(index)) {
                return commonTypeTable[index];
            }
            return commonTypeTable[index] = create<Tp>(std::forward<Args>(args)...);
        }

        template<typename Tp>
        void deallocate(Tp *p) {
            if (!p) return;
            const size_t id = typeid(*p).hash_code();

            const auto it = deleterTable.find(id);
            if (it == deleterTable.end())
                throw std::runtime_error("Unknown type passed to destroy()");

            it->second(p, pool);
        }
    };
} // riddle

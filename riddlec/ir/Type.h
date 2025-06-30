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

#include "Value.h"

namespace riddle {
    class Type {
    public:
        enum class TypeKind {
            Integer,
            FloatPoint,
            Pointer,
            Array,
            Struct,
            Union,
        };

    protected:
        RiddleContext &_ctx;

        TypeKind kind;

        explicit Type(RiddleContext &context, TypeKind kind): _ctx(context), kind(kind) {}

    public:
        virtual ~Type() {
            _ctx.deallocate(this);
        }

        [[nodiscard]] TypeKind getKind() const {
            return kind;
        }

        virtual size_t getSize() = 0;

        [[nodiscard]] RiddleContext &getContext() const {
            return _ctx;
        }
    };

    class IntegerType final : public Type {
        friend RiddleContext;

    protected:
        unsigned bits;

        IntegerType(RiddleContext &context, const unsigned bits): Type(context, TypeKind::Integer), bits(bits) {}

    public:
        static IntegerType *get(RiddleContext &context, const unsigned bits = 32) {
            return context.create<IntegerType>(context, bits);
        }

        size_t getSize() override {
            return bits;
        }
    };

    class FPType : public Type {
    protected:
        unsigned bits;

        FPType(RiddleContext &context, const unsigned bits): Type(context, TypeKind::FloatPoint), bits(bits) {}

        size_t getSize() override {
            return bits;
        }
    };

    class FloatType final : public FPType {
        friend RiddleContext;

    protected:
        explicit FloatType(RiddleContext &context): FPType(context, 32) {}

    public:
        static FloatType *get(RiddleContext &context) {
            return context.create<FloatType>(context);
        }
    };

    class DoubleType final : public FPType {
        friend RiddleContext;

    protected:
        explicit DoubleType(RiddleContext &context): FPType(context, 64) {}

    public:
        static DoubleType *get(RiddleContext &context) {
            return context.create<DoubleType>(context);
        }
    };

    class ArrayType final : public Type {
        friend RiddleContext;

    protected:
        size_t length;
        Type *elementType;

        explicit ArrayType(RiddleContext &context,
                           Type *elementType,
                           const size_t length): Type(context, TypeKind::Array),
                                                 length(length),
                                                 elementType(elementType) {}

    public:
        static ArrayType *get(Type *elementType,
                              const size_t length) {
            if (elementType == nullptr) {
                throw std::logic_error("Element Type is Null");
            }
            auto &ctx = elementType->getContext();
            return ctx.create<ArrayType>(ctx, elementType, length);
        }

        static ArrayType *get(RiddleContext &context,
                              Type *elementType,
                              const size_t length) {
            if (elementType == nullptr) {
                throw std::logic_error("Element Type is Null");
            }
            return context.create<ArrayType>(context, elementType, length);
        }

        size_t getSize() override {
            return elementType->getSize() * length;
        }
    };
}

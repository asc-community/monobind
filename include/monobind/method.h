// MIT License
// 
// Copyright (c) 2020 #Momo
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <monobind/exception_handling.h>
#include <monobind/converters.h>

#include <functional>
#include <array>

namespace monobind
{
    template<typename T>
    struct internal_get_function_type;

    template<typename R, typename... Args >
    struct internal_get_function_type<R(*)(Args...)>
    {
        using type = std::function<R(Args...)>;
        using result_type = R;
    };

    template<typename... Args>
    void internal_init_params(MonoDomain* domain, void** current) { }

    template<typename T, typename... Args>
    void internal_init_params(MonoDomain* domain, void** current, T&& t, Args&&... other)
    {
        using TypeToConvert = typename std::decay<T>::type;
        auto converted = to_mono_converter<TypeToConvert>::convert(domain, std::forward<T>(t));
        static_assert(std::is_pointer<decltype(converted)>::value, "conversion function must return objects by pointer");

        *current = (void*)converted;
        internal_init_params(domain, current + 1, std::forward<Args>(other)...);
    }

    class method
    {
        MonoDomain* m_domain;
        MonoMethod* m_native_ptr;

    public:
        method(MonoDomain* domain, MonoMethod* native_ptr)
            : m_domain(domain), m_native_ptr(native_ptr)
        {
            MONOBIND_ASSERT(m_native_ptr != nullptr);
        }
        
        MonoMethod* get_pointer()
        {
            return m_native_ptr;
        }

        const MonoMethod* get_pointer() const
        {
            return m_native_ptr;
        }

        template<typename... Args>
        decltype(auto) invoke(Args&&... args) const
        {
            return this->operator()(std::forward<Args>(args)...);
        }

        template<typename... Args>
        decltype(auto) operator()(Args&&... args) const
        {
            return this->operator()(static_cast<MonoObject*>(nullptr), std::forward<Args>(args)...);
        }

        template<typename... Args>
        MonoObject* operator()(MonoObject* object, Args&&... args) const
        {
            std::array<void*, sizeof...(args)> params;
            internal_init_params(m_domain, params.data(), std::forward<Args>(args)...);
            return mono_runtime_invoke(m_native_ptr, object, params.data(), nullptr);
        }

        template<typename FuncPointerType>
        typename internal_get_function_type<FuncPointerType>::type as_function()
        {
            return [method = *this](auto&&... args)
            {
                using ResultType = typename internal_get_function_type<FuncPointerType>::result_type;

                MonoObject* result = method.invoke(std::forward<decltype(args)>(args)...);
                if (!std::is_void<ResultType>::value && result == nullptr)
                {
                    throw_exception("mono method returned null");
                }
                return from_mono_converter<ResultType>::convert(method.m_domain, result);
            };
        }

        MonoDomain* get_domain()
        {
            return m_domain;
        }

        const MonoDomain* get_domain() const
        {
            return m_domain;
        }
    };
}
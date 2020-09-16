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
#include <monobind/converters_fwd.h>

#include <functional>
#include <array>

namespace monobind
{
    class object;

    template<typename T>
    struct internal_get_function_type;

    template<typename R, typename... Args >
    struct internal_get_function_type<R(Args...)>
    {
        using type = std::function<R(Args...)>;
        using result_type = R;

        static constexpr size_t argument_count = sizeof...(Args);
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

        MonoMethod* get_pointer() const
        {
            return m_native_ptr;
        }

        MonoDomain* get_domain() const
        {
            return m_domain;
        }

        template<typename T, typename... Args>
        T invoke_instance(MonoObject* self, Args&&... args) const
        {
            std::array<void*, sizeof...(args)> params;
            internal_init_params(m_domain, params.data(), std::forward<Args>(args)...);
            MonoObject* result = mono_runtime_invoke(m_native_ptr, self, params.data(), nullptr);
            if (!std::is_void<T>::value && result == nullptr)
            {
                throw_exception("mono method returned null");
            }
            return from_mono_converter<T>::convert(m_domain, result);
        }

        template<typename T, typename... Args>
        decltype(auto) invoke_instance(const object& object, Args&&... args) const
        {
            return invoke_instance<T>(object.get_pointer(), std::forward<Args>(args)...);
        }

        template<typename T, typename... Args>
        decltype(auto) invoke_static(Args&&... args) const
        {
            return invoke_instance<T>(static_cast<MonoObject*>(nullptr), std::forward<Args>(args)...);
        }

        template<typename FunctionSignature>
        auto as_function()
        {
            using FunctorInfo = internal_get_function_type<FunctionSignature>;
            using ResultType = typename FunctorInfo::result_type;
            using Functor = typename FunctorInfo::type;

            return Functor([method = *this](auto&&... args)
            {
                return method.invoke_static<ResultType>(std::forward<decltype(args)>(args)...);
            });
        }
    };
}
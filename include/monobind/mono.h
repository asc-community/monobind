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
#include <monobind/domain.h>

#include <string>
#include <tuple>

namespace monobind
{
    #define MONOBIND_CALLABLE(func_name) [](auto... args) -> decltype(auto) { return func_name(std::forward<decltype(args)>(args)...); }

    template<typename T>
    struct internal_convert_type_to_mono
    {
        using result = typename std::conditional<
            can_be_trivially_converted<T>::value,
            T, decltype(to_mono_converter<T>::convert(std::declval<MonoDomain*>(), std::declval<T>()))
        >::type;
    };

    template<>
    struct internal_convert_type_to_mono<void>
    {
        using result = void;
    };

    template<typename T>
    class internal_convert_tuple_types_to_mono;

    template<typename T>
    class internal_convert_tuple_types_to_mono<std::tuple<T>>
    {
        using result_type = typename internal_convert_type_to_mono<T>::result;
    public:
        using result = std::tuple<result_type>;
    };

    template<typename T, typename... Args>
    class internal_convert_tuple_types_to_mono<std::tuple<T, Args...>>
    {
        template<typename... Args1, typename... Args2>
        static auto merge_tuples(std::tuple<Args1...>*, std::tuple<Args2...>*) -> std::tuple<Args1..., Args2...>;

        using result_head_tuple = typename internal_convert_tuple_types_to_mono<std::tuple<T>>::result;
        using result_tail_tuple = typename internal_convert_tuple_types_to_mono<std::tuple<Args...>>::result;
    public:
        using result = decltype(merge_tuples((result_head_tuple*)nullptr, (result_tail_tuple*)nullptr));
    };

    template<>
    class internal_convert_tuple_types_to_mono<std::tuple<>>
    {
    public:
        using result = std::tuple<>;
    };

    template<typename T>
    struct internal_function_wrapper;

    template<typename R, typename... Args>
    class internal_function_wrapper<R(Args...)>
    {
        template<typename F, typename MonoReturnType, typename... MonoArgs>
        static auto invoke_inner_function(MonoArgs&&... args)
            -> typename std::enable_if<!std::is_void<MonoReturnType>::value, MonoReturnType>::type
        {
            MonoDomain* domain = get_current_domain();
            void* dummy = nullptr;
            auto result = reinterpret_cast<F*>(dummy)->operator()(from_mono_converter<Args>::convert(domain, args)...);
            return to_mono_converter<R>::convert(domain, std::move(result));
        }

        template<typename F, typename MonoReturnType, typename... MonoArgs>
        static auto invoke_inner_function(MonoArgs&&... args) 
            -> typename std::enable_if<std::is_void<MonoReturnType>::value, void>::type
        {
            MonoDomain* domain = get_current_domain();
            void* dummy = nullptr;
            reinterpret_cast<F*>(dummy)->operator()(from_mono_converter<Args>::convert(domain, args)...);
        }

        using argument_list_tuple = typename internal_convert_tuple_types_to_mono<std::tuple<Args...>>::result;
        using return_type = typename internal_convert_type_to_mono<R>::result;

        template<typename F, typename MonoReturnType, typename... MonoArgs>
        static auto get_impl(MonoReturnType*, std::tuple<MonoArgs...>*)
        {
            using PureFuncType = MonoReturnType(*)(MonoArgs...);
            PureFuncType pure_func = [](MonoArgs... args) -> MonoReturnType
            {
                return invoke_inner_function<F, MonoReturnType, MonoArgs...>(std::forward<MonoArgs>(args)...);
            };
            return pure_func;
        }

    public:
        template<typename F>
        static auto get()
        { 
            static_assert(std::is_convertible<F, R(*)(Args...)>::value, "functor type must be convertible to function pointer");
            return get_impl<F>((return_type*)nullptr, (argument_list_tuple*)nullptr);
        }
    };

    class mono
    {
        std::string m_mono_root_dir;
        MonoDomain* m_domain = nullptr;

    public:
        mono(const char* mono_root_dir)
            : m_mono_root_dir(mono_root_dir)
        {
            mono_set_dirs((m_mono_root_dir + "/lib").c_str(), (m_mono_root_dir + "/etc").c_str());
        }

        mono(const char* mono_root_dir, const char* mono_assembly_dir, const char* mono_config_dir)
            : m_mono_root_dir(mono_root_dir)
        {
            mono_set_dirs(mono_assembly_dir, mono_config_dir);
        }

        mono(const mono&) = delete;
        mono& operator=(const mono&) = delete;

        mono(mono&& other) noexcept
            : m_mono_root_dir(std::move(other.m_mono_root_dir)), m_domain(other.m_domain)
        {
            other.m_domain = nullptr;
        }

        mono& operator=(mono&& other) noexcept
        {
            m_mono_root_dir = std::move(other.m_mono_root_dir);
            m_domain = other.m_domain;

            other.m_domain = nullptr;

            return *this;
        }

        ~mono()
        {
            if (m_domain != nullptr)
            {
                mono_jit_cleanup(m_domain);
            }
        }
        
        void init_jit(const char* app_name)
        {
            m_domain = mono_jit_init(app_name);
            if (m_domain == nullptr)
            {
                throw_exception("mono_jit_init failed");
            }
        }

        void init_jit(const char* app_name, const char* framework_version)
        {
            m_domain = mono_jit_init_version(app_name, framework_version);
            if (m_domain == nullptr)
            {
                throw_exception("mono_jit_init failed");
            }
        }

        MonoDomain* get_domain() const
        {
            return m_domain;
        }

        const std::string& get_root_dir() const
        {
            return m_mono_root_dir;
        }

        template<typename FunctionSignature, typename F>
        void add_internal_call(const char* signature, F&& f)
        {
            auto wrapper = internal_function_wrapper<FunctionSignature>::get<F>();
            mono_add_internal_call(signature, static_cast<const void*>(wrapper));
        }
    };
}
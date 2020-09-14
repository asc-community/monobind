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

#include <monobind/mono_api_include.h>
#include <monobind/exception_handling.h>

#include <string>

namespace monobind
{
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

        MonoDomain* get_domain()
        {
            return m_domain;
        }

        const MonoDomain* get_domain() const
        {
            return m_domain;
        }

        const std::string& get_root_dir() const
        {
            return m_mono_root_dir;
        }

        template<typename R, typename... Args>
        void add_internal_call(const char* signature, R(*func)(Args...))
        {
            mono_add_internal_call(signature, static_cast<const void*>(func));
        }

        template<typename CStyleFuncType, typename F>
        void add_internal_call(const char* signature, F&& f)
        {
            static_assert(std::is_convertible<F, CStyleFuncType>::value, "functor must be convertable to c-style pointer");
            CStyleFuncType c_style_func = f;
            mono_add_internal_call(signature, static_cast<const void*>(c_style_func));
        }
    };
}
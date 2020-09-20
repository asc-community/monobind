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
#include <monobind/method.h>

#include <map>

namespace monobind
{
    class assembly
    {
        MonoDomain* m_domain = nullptr;
        MonoAssembly* m_assembly = nullptr;
        MonoImage* m_assembly_image = nullptr;

        mutable std::map<std::string, MonoMethod*, std::less<>> m_method_cache;

        bool add_to_cache(const char* method_signature) const
        {
            MonoMethodDesc* desc = mono_method_desc_new(method_signature, false);
            if (desc == nullptr)
            {
                throw_exception("invalid method signature");
            }
            MonoMethod* method_ptr = mono_method_desc_search_in_image(desc, m_assembly_image);
            mono_method_desc_free(desc);
            if (method_ptr != nullptr)
            {
                m_method_cache.emplace(method_signature, method_ptr);
            }
            return method_ptr != nullptr;
        }
    public:
        assembly(MonoDomain* domain, const char* path)
            : m_domain(domain)
        {
            MONOBIND_ASSERT(domain != nullptr);

            m_assembly = mono_domain_assembly_open(domain, path);
            if (m_assembly == nullptr)
            {
                throw_exception("could not open assembly");
            }
            
            m_assembly_image = mono_assembly_get_image(m_assembly);
            if (m_assembly_image == nullptr)
            {
                m_assembly = nullptr;
                throw_exception("could not get assembly image");
            }
        }

        MonoAssembly* get_assembly() const
        {
            return m_assembly;
        }

        MonoImage* get_image() const
        {
            return m_assembly_image;
        }

        method get_method(const char* signature) const
        {
            if (!has_method(signature))
            {
                throw_exception("could not find method in assembly");
                return method(m_domain, nullptr);
            }
            else
            {
                return method(m_domain, m_method_cache[signature]);
            }
        }

        bool has_method(const char* signature) const
        {
            auto it = m_method_cache.find(signature);
            if (it != m_method_cache.end())
            {
                return true;
            }
            else
            {
                bool added = add_to_cache(signature);
                return added;
            }
        }
    };
}
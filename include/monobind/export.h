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

#include <monobind/converters_fwd.h>
#include <monobind/code_generator.h>

#define MONOBIND_GET(type, method_name) [](uintptr_t x) -> decltype(auto) { return reinterpret_cast<type*>(x)->method_name(); }
#define MONOBIND_SET(type, method_name) [](uintptr_t x, auto arg) -> void { reinterpret_cast<type*>(x)->method_name(std::move(arg)); }
#define MONOBIND_METHOD(type, method_name) [](uintptr_t x, auto... args) -> decltype(auto) { return reinterpret_cast<type*>(x)->method_name(std::move(args)...); }

namespace monobind
{
    template<typename T>
    class export_struct
    {
        code_generator& m_generator;
    public:
        export_struct(const char* name, code_generator& gen)
            : m_generator(gen)
        {
            m_generator.generate_struct_header<T>(name);
        }

        ~export_struct()
        {
            m_generator.generate_footer();
        }

        template<typename X>
        auto& field(const char* name, X T::* f)
        {
            m_generator.generate_struct_field(name, f);
            return *this;
        }

        template<typename X>
        auto& readonly_field(const char* name, X T::* f)
        {
            m_generator.generate_readonly_struct_field(name, f);
            return *this;
        }

        template<typename GetCallable, typename SetCallable>
        auto& property(const char* name, GetCallable&& get, SetCallable&& set)
        {
            m_generator.generate_struct_property<T>(name, std::forward<GetCallable>(get), std::forward<SetCallable>(set));
            return *this;
        }

        template<typename GetCallable>
        auto& property(const char* name, GetCallable&& get)
        {
            m_generator.generate_readonly_struct_property<T>(name, std::forward<GetCallable>(get));
            return *this;
        }
    };

    template<typename T>
    class export_class
    {
        code_generator& m_generator;
    public:
        export_class(const char* name, code_generator& gen)
            : m_generator(gen)
        {
            m_generator.generate_class_header<T>(name);
        }

        ~export_class()
        {
            m_generator.generate_footer();
        }
        
        template<typename X>
        auto& field(const char* name, X T::* f)
        {
            m_generator.generate_class_field(name, f);
            return *this;
        }

        template<typename X>
        auto& readonly_field(const char* name, X T::* f)
        {
            m_generator.generate_readonly_class_field(name, f);
            return *this;
        }

        template<typename GetCallable, typename SetCallable>
        auto& property(const char* name, GetCallable&& get, SetCallable&& set)
        {
            m_generator.generate_class_property(name, std::forward<GetCallable>(get), std::forward<SetCallable>(set));
            return *this;
        }

        template<typename GetCallable>
        auto& property(const char* name, GetCallable&& get)
        {
            m_generator.generate_readonly_class_property(name, std::forward<GetCallable>(get));
            return *this;
        }
    };
}
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

#include <monobind/method.h>

namespace monobind
{
    class method_view
    {
        MonoClass* m_class = nullptr;
        MonoDomain* m_domain = nullptr;
    public:
        class method_iterator
        {
            void* m_iter = nullptr;
            MonoDomain* m_domain = nullptr;
            MonoClass* m_class = nullptr;
            method m_method;
        public:
            method_iterator()
                : m_method(nullptr, nullptr)
            {

            }

            method_iterator(MonoDomain* domain, MonoClass* cl, void* iter)
                : m_iter(iter), m_domain(domain), m_class(cl), m_method(m_domain, mono_class_get_methods(m_class, &m_iter))
            {

            }

            const method& operator*() const
            {
                return m_method;
            }

            const method* operator->() const
            {
                return std::addressof(m_method);
            }

            bool operator==(const method_iterator& it) const
            {
                return m_method.get_pointer() == it.m_method.get_pointer();
            }

            bool operator!=(const method_iterator& it) const
            {
                return !(*this == it);
            }

            method_iterator& operator++()
            {
                auto m = mono_class_get_methods(m_class, &m_iter);
                m_method = method(m_domain, m);
                return *this;
            }

            method_iterator operator++(int)
            {
                auto copy = *this;
                ++(*this);
                return copy;
            }
        };

        method_view(MonoDomain* domain, MonoClass* cl)
            : m_class(cl), m_domain(domain)
        {

        }

        auto begin() const
        {
            return method_iterator(m_domain, m_class, nullptr);
        }

        auto end() const
        {
            return method_iterator();
        }

        size_t size() const
        {
            return (size_t)mono_class_num_methods(m_class);
        }
    };
}
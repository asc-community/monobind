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

#include <monobind/mono_api_include.h>

namespace monobind
{
    class property_view
    {
        MonoClass* m_class;

    public:
        class property_iterator
        {
            void* m_iter = nullptr;
            MonoClass* m_class = nullptr;
            MonoProperty* m_property = nullptr;
        public:
            property_iterator() = default;

            property_iterator(MonoClass* cl, void* iter)
                : m_iter(iter), m_class(cl)
            {
                m_property = mono_class_get_properties(m_class, &m_iter);
            }

            const char* operator*() const
            {
                return mono_property_get_name(m_property);
            }

            bool operator==(const property_iterator& it) const
            {
                return m_property == it.m_property;
            }

            bool operator!=(const property_iterator& it) const
            {
                return !(*this == it);
            }

            property_iterator& operator++()
            {
                m_property = mono_class_get_properties(m_class, &m_iter);
                return *this;
            }

            property_iterator operator++(int)
            {
                auto copy = *this;
                ++(*this);
                return copy;
            }
        };

        property_view(MonoClass* cl)
            : m_class(cl)
        {

        }

        auto begin() const
        {
            return property_iterator(m_class, nullptr);
        }

        auto end() const
        {
            return property_iterator();
        }

        size_t size() const
        {
            return (size_t)mono_class_num_properties(m_class);
        }
    };
}
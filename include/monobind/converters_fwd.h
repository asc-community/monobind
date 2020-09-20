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
#include <memory>

namespace monobind
{
    template<typename T>
    struct to_mono_converter
    {
        static const T* convert(MonoDomain* domain, const T& t)
        {
            return std::addressof(t);
        }
    };

    template<typename T>
    struct from_mono_converter
    {
        static T convert(MonoDomain* domain, T t)
        {
            return t;
        }

        static T convert(MonoDomain* domain, MonoObject* t)
        {
            return *(T*)mono_object_unbox(t);
        }
    };

    template<typename T>
    struct can_be_trivially_converted
    {
        static constexpr size_t value = std::is_standard_layout<T>::value;
    };
}
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

#include <monobind/object.h>

#include <ostream>

namespace monobind
{
    std::ostream& operator<<(std::ostream& out, const object& obj)
    {
        out << obj.to_string();
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const object::field_wrapper& wrapper)
    {
        out << wrapper.get();
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const class_type::static_field_wrapper& wrapper)
    {
        out << wrapper.as<object>();
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const method& m)
    {
        out << m.get_signature();
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const class_type& cl)
    {
        out << cl.get_name();
        return out;
    }
}
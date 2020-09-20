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
    struct type_accessor
    {
        static MonoClass* get_type(MonoObject* obj)
        {
            return mono_object_get_class(obj);
        }

        static MonoClass* get_type(MonoString* str)
        {
            return mono_get_string_class();
        }

        static MonoClass* get_type(uint16_t* t)
        {
            return mono_get_uint16_class();
        }

        static MonoClass* get_type(int16_t* t)
        {
            return mono_get_int16_class();
        }

        static MonoClass* get_type(uint32_t* t)
        {
            return mono_get_uint32_class();
        }

        static MonoClass* get_type(int32_t* t)
        {
            return mono_get_int32_class();
        }

        static MonoClass* get_type(uint64_t* t)
        {
            return mono_get_uint64_class();
        }

        static MonoClass* get_type(int64_t* t)
        {
            return mono_get_int64_class();
        }

        static  MonoClass* get_type(char* t)
        {
            return mono_get_byte_class();
        }

        static MonoClass* get_type(wchar_t* t)
        {
            return mono_get_char_class();
        }

        static MonoClass* get_type(bool* t)
        {
            return mono_get_boolean_class();
        }
    };
}
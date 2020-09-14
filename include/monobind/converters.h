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
#include <string>

namespace monobind
{
    template<typename T>
    struct to_mono_converter
    {
        static auto convert(MonoDomain* domain, const T& t)
        {
            return std::addressof(t);
        }
    };

    template<typename ManagedType, typename UnmanagedType>
    struct from_mono_converter
    {
        static UnmanagedType convert(MonoDomain* domain, ManagedType* t)
        {
            return (UnmanagedType)t;
        }
    };

    template<>
    struct to_mono_converter<const char*>
    {
        static MonoString* convert(MonoDomain* domain, const char* str)
        {
            return mono_string_new(domain, str);
        }
    };

    template<>
    struct to_mono_converter<char*>
    {
        static MonoString* convert(MonoDomain* domain, const char* str)
        {
            return mono_string_new(domain, str);
        }
    };

    template<>
    struct to_mono_converter<std::string>
    {
        static MonoString* convert(MonoDomain* domain, const std::string& str)
        {
            return mono_string_new(domain, str.c_str());
        }
    };

    template<>
    struct from_mono_converter<MonoString*, std::string>
    {
        static std::string convert(MonoDomain* domain, MonoString* str)
        {
            std::string result;
            MonoError err;
            char* utf8str = mono_string_to_utf8_checked(str, &err);
            if (err.error_code == MONO_ERROR_NONE)
            {
                result = utf8str;
                mono_free(utf8str);
            }
            return result;
        }
    };

    template<>
    struct to_mono_converter<std::wstring>
    {
        static MonoString* convert(MonoDomain* domain, const std::wstring& str)
        {
            return mono_string_new_utf16(domain, str.c_str(), (int32_t)str.size());
        }
    };

    template<>
    struct from_mono_converter<MonoString*, std::wstring>
    {
        static std::wstring convert(MonoDomain* domain, MonoString* str)
        {
            return mono_string_chars(str);
        }
    };
}
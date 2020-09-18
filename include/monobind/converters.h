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
#include <monobind/type_accessor.h>
#include <monobind/converters_fwd.h>
#include <monobind/object.h>

#include <string>
#include <vector>
#include <array>

namespace monobind
{
    template<typename T>
    auto internal_copy_to_mono_array(MonoDomain* domain, const std::vector<T>& vec, MonoArray* arr)
        -> typename std::enable_if<std::is_standard_layout<T>::value>::type
    {
        for (size_t i = 0; i < vec.size(); i++)
        {
            auto converted = to_mono_converter<T>::convert(domain, vec[i]);
            mono_array_set(arr, T, i, converted);
        }
    }

    template<typename T>
    static auto internal_copy_to_mono_array(MonoDomain* domain, const std::vector<T>& vec, MonoArray* arr)
        -> typename std::enable_if<!std::is_standard_layout<T>::value>::type
    {
        for (size_t i = 0; i < vec.size(); i++)
        {
            auto converted = to_mono_converter<T>::convert(domain, vec[i]);
            mono_array_setref(arr, i, converted);
        }
    }

    template<>
    struct from_mono_converter<void>
    {
        static void convert(MonoDomain* domain, MonoObject* t)
        {
            (void)t;
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

    template<typename T>
    struct from_mono_converter<std::vector<T>>
    {
        static std::vector<T> convert(MonoDomain* domain, MonoArray* arr)
        {
            size_t size = mono_array_length(arr);
            std::vector<T> vec(size);
            for (size_t i = 0; i < size; i++)
            {
                auto object = mono_array_get(arr, MonoObject*, i);
                vec[i] = from_mono_converter<T>::convert(domain, object);
            }
            return std::move(vec);
        }

        static std::vector<T> convert(MonoDomain* domain, MonoObject* obj)
        {
            MonoArray* arr = reinterpret_cast<MonoArray*>(obj);
            return convert(domain, arr);
        }
    };

    template<typename T>
    struct to_mono_converter<std::vector<T>>
    {
        static MonoArray* convert(MonoDomain* domain, const std::vector<T>& vec)
        {
            if (vec.empty()) return nullptr;

            auto converted = to_mono_converter<T>::convert(domain, vec.front());
            MonoClass* class_type = type_accessor::get_type(converted);
            MonoArray* arr = mono_array_new(domain, class_type, vec.size());

            internal_copy_to_mono_array(domain, vec, arr);
            return arr;
        }
    };

    template<typename T, size_t N>
    struct from_mono_converter<std::array<T, N>>
    {
        static std::array<T, N> convert(MonoDomain* domain, MonoArray* arr)
        {
            size_t size = mono_array_length(arr);
            std::array<T, N> vec;
            for (size_t i = 0; i < (N < size ? N : size); i++)
            {
                auto object = mono_array_get(arr, MonoObject*, i);
                vec[i] = from_mono_converter<T>::convert(domain, object);
            }
            return vec;
        }

        static std::array<T, N> convert(MonoDomain* domain, MonoObject* obj)
        {
            MonoArray* arr = reinterpret_cast<MonoArray*>(obj);
            return convert(domain, arr);
        }
    };

    template<typename T, size_t N>
    struct to_mono_converter<std::array<T, N>>
    {
        static MonoArray* convert(MonoDomain* domain, const std::array<T, N>& vec)
        {
            if (vec.empty()) return nullptr;

            auto converted = to_mono_converter<T>::convert(domain, vec.front());
            MonoClass* class_type = type_accessor::get_type(converted);
            MonoArray* arr = mono_array_new(domain, class_type, vec.size());

            internal_copy_to_mono_array(domain, vec, arr);
            return arr;
        }
    };

    template<>
    struct from_mono_converter<std::string>
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

        static std::string convert(MonoDomain* domain, MonoObject* obj)
        {
            MonoObject* exc = nullptr;
            MonoString* str = mono_object_to_string(obj, &exc);
            if (exc != nullptr)
            {
                throw_exception("cannot convert object to string");
            }
            return convert(domain, str);
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
    struct to_mono_converter<std::wstring>
    {
        static MonoString* convert(MonoDomain* domain, const std::wstring& str)
        {
            return mono_string_new_utf16(domain, str.c_str(), (int32_t)str.size());
        }
    };

    template<>
    struct from_mono_converter<std::wstring>
    {
        static std::wstring convert(MonoDomain* domain, MonoString* str)
        {
            return mono_string_chars(str);
        }

        static std::wstring convert(MonoDomain* domain, MonoObject* obj)
        {
            MonoObject* exc = nullptr;
            MonoString* str = mono_object_to_string(obj, &exc);
            if (exc != nullptr)
            {
                throw_exception("cannot convert object to string");
            }
            return convert(domain, str);
        }
    };

    template<>
    struct to_mono_converter<object>
    {
        static MonoObject* convert(MonoDomain* domain, const object& obj)
        {
            return obj.get_pointer();
        }
    };

    template<>
    struct from_mono_converter<object>
    {
        static object convert(MonoDomain* domain, MonoObject* obj)
        {
            return object(obj);
        }
    };

    inline std::string to_string(MonoString* str)
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

    inline std::wstring to_wstring(MonoString* str)
    {
        std::wstring result;
        wchar_t* utf16str = mono_string_chars(str);
        result = utf16str;
        return result;
    }

    inline object to_object(MonoObject* obj)
    {
        return object(obj);
    }
}
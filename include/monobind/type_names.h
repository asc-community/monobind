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

#include <cstdint>

#define MONOBIND_CHAR_TYPE   "byte"
#define MONOBIND_WCHAR_TYPE  "char"
#define MONOBIND_SHORT_TYPE  "int16"
#define MONOBIND_USHORT_TYPE "uint16"
#define MONOBIND_INT_TYPE    "int"
#define MONOBIND_UINT_TYPE   "uint"
#define MONOBIND_LONG_TYPE   "long"
#define MONOBIND_ULONG_TYPE  "ulong"
#define MONOBIND_FLOAT_TYPE  "single"
#define MONOBIND_DOUBLE_TYPE "double"
#define MONOBIND_BOOL_TYPE   "bool"
#define MONOBIND_STRING_TYPE "string"

#define MONOBIND_INT8_TYPE   MONOBIND_CHAR_TYPE
#define MONOBIND_UINT8_TYPE  MONOBIND_CHAR_TYPE
#define MONOBIND_SINGLE_TYPE MONOBIND_FLOAT_TYPE
#define MONOBIND_INT16_TYPE  MONOBIND_SHORT_TYPE
#define MONOBIND_UINT16_TYPE MONOBIND_USHORT_TYPE
#define MONOBIND_INT32_TYPE  MONOBIND_SHORT_TYPE
#define MONOBIND_UINT32_TYPE MONOBIND_INT_TYPE
#define MONOBIND_INT64_TYPE  MONOBIND_LONG_TYPE
#define MONOBIND_UINT64_TYPE MONOBIND_ULONG_TYPE
#define MONOBIND_WSTRING_TYPE MONOBIND_STRING_TYPE

#include <string>

namespace monobind
{
    template<typename T>
    struct type_name;

    template<>
    struct type_name<char>
    {
        static constexpr char* value = MONOBIND_CHAR_TYPE;
    };

    template<>
    struct type_name<wchar_t>
    {
        static constexpr char* value = MONOBIND_WCHAR_TYPE;
    };

    template<>
    struct type_name<int8_t>
    {
        static constexpr char* value = MONOBIND_INT8_TYPE;
    };

    template<>
    struct type_name<int16_t>
    {
        static constexpr char* value = MONOBIND_SHORT_TYPE;
    };

    template<>
    struct type_name<int32_t>
    {
        static constexpr char* value = MONOBIND_INT_TYPE;
    };

    template<>
    struct type_name<int64_t>
    {
        static constexpr char* value = MONOBIND_LONG_TYPE;
    };

    template<>
    struct type_name<float>
    {
        static constexpr char* value = MONOBIND_FLOAT_TYPE;
    };

    template<>
    struct type_name<double>
    {
        static constexpr char* value = MONOBIND_DOUBLE_TYPE;
    };

    template<>
    struct type_name<bool>
    {
        static constexpr char* value = MONOBIND_BOOL_TYPE;
    };

    template<>
    struct type_name<uint8_t>
    {
        static constexpr char* value = MONOBIND_UINT8_TYPE;
    };
    
    template<>
    struct type_name<uint16_t>
    {
        static constexpr char* value = MONOBIND_USHORT_TYPE;
    };

    template<>
    struct type_name<uint32_t>
    {
        static constexpr char* value = MONOBIND_UINT_TYPE;
    };

    template<>
    struct type_name<uint64_t>
    {
        static constexpr char* value = MONOBIND_ULONG_TYPE;
    };

    template<>
    struct type_name<std::string>
    {
        static constexpr char* value = MONOBIND_STRING_TYPE;
    };

    template<>
    struct type_name<std::wstring>
    {
        static constexpr char* value = MONOBIND_WSTRING_TYPE;
    };
}
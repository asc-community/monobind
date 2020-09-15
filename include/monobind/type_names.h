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

#define MONOBIND_INT8_TYPE   MONOBIND_CHAR_TYPE
#define MONOBIND_UINT8_TYPE  MONOBIND_CHAR_TYPE
#define MONOBIND_SINGLE_TYPE MONOBIND_FLOAT_TYPE
#define MONOBIND_INT16_TYPE  MONOBIND_SHORT_TYPE
#define MONOBIND_UINT16_TYPE MONOBIND_USHORT_TYPE
#define MONOBIND_INT32_TYPE  MONOBIND_SHORT_TYPE
#define MONOBIND_UINT32_TYPE MONOBIND_INT_TYPE
#define MONOBIND_INT64_TYPE  MONOBIND_LONG_TYPE
#define MONOBIND_UINT64_TYPE MONOBIND_ULONG_TYPE

namespace monobind
{
    template<typename T>
    struct type_name;

    template<>
    struct type_name<char>
    {
        constexpr char* value = MONOBIND_CHAR_TYPE;
    };

    template<>
    struct type_name<wchar_t>
    {
        constexpr char* value = MONOBIND_WCHAR_TYPE;
    };

    template<>
    struct type_name<int16_t>
    {
        constexpr char* value = MONOBIND_SHORT_TYPE;
    };

    template<>
    struct type_name<int32_t>
    {
        constexpr char* value = MONOBIND_INT_TYPE;
    };

    template<>
    struct type_name<int64_t>
    {
        constexpr char* value = MONOBIND_LONG_TYPE;
    };

    template<>
    struct type_name<float>
    {
        constexpr char* value = MONOBIND_FLOAT_TYPE;
    };

    template<>
    struct type_name<double>
    {
        constexpr char* value = MONOBIND_DOUBLE_TYPE;
    };

    template<>
    struct type_name<bool>
    {
        constexpr char* value = MONOBIND_BOOL_TYPE;
    };
    
    template<>
    struct type_name<uint16_t>
    {
        constexpr char* value = MONOBIND_USHORT_TYPE;
    };

    template<>
    struct type_name<uint32_t>
    {
        constexpr char* value = MONOBIND_UINT_TYPE;
    };

    template<>
    struct type_name<uint64_t>
    {
        constexpr char* value = MONOBIND_ULONG_TYPE;
    };
}
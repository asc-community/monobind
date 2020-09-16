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
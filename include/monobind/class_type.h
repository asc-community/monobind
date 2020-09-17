#include <monobind/mono_api_include.h>

namespace monobind
{
    class class_type
    {
        MonoClass* m_class = nullptr;
    public:
        class_type(MonoImage* image, const char* namespace_name, const char* class_name)
        {
            m_class = mono_class_from_name(image, namespace_name, class_name);
        }

        class_type(MonoClass* cl)
            : m_class(cl)
        {

        }

        class_type(MonoType* type)
            : m_class(mono_type_get_class(type))
        {
            
        }

        MonoClass* get_pointer() const
        {
            return m_class;
        }

        MonoClassField* get_field(const char* field_name) const
        {
            return mono_class_get_field_from_name(m_class, field_name);
        }

        MonoMethod* get_method(const char* name) const
        {
            auto desc = mono_method_desc_new(name, false);
            if (desc == nullptr)
            {
                throw_exception("invalid method signature");
            }
            MonoMethod* m = mono_method_desc_search_in_class(desc, m_class);
            mono_method_desc_free(desc);
            if (m == nullptr)
            {
                throw_exception("could not find method in class");
            }
            return m;
        }
    };
}
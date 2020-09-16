#pragma once

#include <monobind/method.h>

#include <map>
#include <string>

namespace monobind
{
    class object
    {
        class field_wrapper
        {
            MonoDomain* m_domain;
            MonoObject* m_parent;
            MonoClassField* m_field;
        public:
            field_wrapper(MonoDomain* domain, MonoObject* parent, MonoClassField* field)
                : m_domain(domain), m_parent(parent), m_field(field)
            {

            }

            MonoObject* get() const
            {
                return mono_field_get_value_object(m_domain, m_field, m_parent);
            }

            void set(void* obj)
            {
                mono_field_set_value(m_parent, m_field, obj);
            }

            template<typename T>
            void operator=(T&& value)
            {
                set(to_mono_converter<T>::convert(m_domain, std::forward<T>(value)));
            }

            template<typename T>
            operator T() const
            {
                return from_mono_converter<T>::convert(m_domain, get());
            }
        };

        mutable std::map<std::string, MonoClassField*, std::less<>> m_field_cache;
        mutable std::map<std::pair<std::string, size_t>, MonoMethod*, std::less<>> m_method_cache;

        MonoObject* m_object = nullptr;
        MonoDomain* m_domain = nullptr;

        static void alloc_object(MonoDomain* domain, MonoObject** obj, MonoClass* class_type)
        {
            *obj = mono_object_new(domain, class_type);
        }
    public:
        object(MonoObject* object)
            : m_object(object), m_domain(mono_object_get_domain(object))
        {

        }

        object(MonoDomain* domain, MonoClass* class_type)
            : m_domain(domain)
        {
            alloc_object(m_domain, &m_object, class_type);
        }

        object(field_wrapper wrapper)
            : object(wrapper.get())
        {

        }

        MonoClass* get_class() const
        {
            return mono_object_get_class(m_object);
        }

        MonoObject* get_pointer() const
        {
            return m_object;
        }

        MonoClassField* get_field(const char* field_name) const
        {
            auto it = m_field_cache.find(field_name);
            if (it != m_field_cache.end())
            {
                return it->second;
            }
            else
            {
                MonoClass* class_type = get_class();
                MonoClassField* field = mono_class_get_field_from_name(class_type, field_name);
                if (field == nullptr)
                {
                    throw_exception("could not find field in object");
                }
                m_field_cache[field_name] = field;
                return field;
            }
        }

        field_wrapper operator[](const char* field_name) const
        {
            auto field = get_field(field_name);
            MonoType* field_type = mono_field_get_type(field);
            MonoClass* field_class = mono_class_from_mono_type(field_type);
            return field_wrapper(m_domain, m_object, field);
        }

        MonoMethod* get_method_pointer(const char* name, size_t arg_count) const
        {
            auto it = m_method_cache.find({ name, arg_count });
            if (it != m_method_cache.end())
            {
                return it->second;
            }
            else
            {
                MonoClass* class_type = get_class();
                MonoMethod* method_type = mono_class_get_method_from_name(class_type, name, (int)arg_count);
                if (method_type == nullptr)
                {
                    throw_exception("could not find method in class");
                }
                m_method_cache[{name, arg_count}] = method_type;
                return method_type;
            }
        }

        template<typename CStyleFunc>
        auto get_method(const char* method_name)
        {
            using FunctorTraits = internal_get_function_type<CStyleFunc>;
            auto method_type = get_method_pointer(method_name, FunctorTraits::argument_count);
            return method(m_domain, method_type).as_function<CStyleFunc>();
        }

        template<typename T>
        T as()
        {
            return from_mono_converter<T>::convert(m_domain, m_object);
        }
    };
}
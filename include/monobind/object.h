#pragma once

#include <monobind/method.h>
#include <monobind/class_type.h>

#include <map>
#include <string>

namespace monobind
{
    class object
    {
        mutable std::map<std::string, MonoClassField*, std::less<>> m_field_cache;
        mutable std::map<std::string, MonoMethod*, std::less<>> m_method_cache;

        MonoObject* m_object = nullptr;
        MonoDomain* m_domain = nullptr;
        
        static void alloc_object(MonoDomain* domain, MonoObject** obj, MonoClass* class_type)
        {
            *obj = mono_object_new(domain, class_type);
        }

    public:
        class field_wrapper
        {
            MonoDomain* m_domain = nullptr;
            MonoObject* m_parent = nullptr;
            MonoClassField* m_field = nullptr;

            MonoObject* get_underlying_object() const
            {
                return mono_field_get_value_object(m_domain, m_field, m_parent);
            }
        public:
            field_wrapper(MonoDomain* domain, MonoObject* parent, MonoClassField* field)
                : m_domain(domain), m_parent(parent), m_field(field)
            {

            }

            object get() const
            {
                return object(get_underlying_object());
            }

            template<typename T>
            void set(T&& value)
            {
                auto obj = to_mono_converter<T>::convert(m_domain, std::forward<T>(value));
                mono_field_set_value(m_parent, m_field, (void*)obj);
            }

            template<typename T>
            void operator=(T&& value)
            {
                set(std::forward<T>(value));
            }

            template<typename T>
            operator T() const
            {
                return as<T>();
            }

            template<typename T>
            T as() const
            {
                return from_mono_converter<T>::convert(m_domain, get_underlying_object());
            }
        };

        object(MonoDomain* domain, const class_type& class_t)
            : object(domain, class_t.get_pointer())
        {
          
        }

        object(MonoObject* object)
            : m_object(object), m_domain(mono_object_get_domain(object))
        {

        }

        object(MonoDomain* domain, MonoClass* class_t)
            : m_domain(domain)
        {
            alloc_object(m_domain, &m_object, class_t);
            mono_runtime_object_init(m_object);
        }

        template<typename... Args>
        object(MonoDomain* domain, const class_type& class_t, const char* construtor_signature, Args&&... args)
            : m_domain(domain)
        {
            alloc_object(m_domain, &m_object, class_t.get_pointer());
            MonoMethod* ctor = get_method_pointer(construtor_signature);
            if (ctor == nullptr)
            {
                throw_exception("could not found appropriate constructor for given class");
            }
            method constructor(m_domain, ctor);
            constructor.invoke_instance<void(Args...)>(*this, std::forward<Args>(args)...);
        }

        object(field_wrapper wrapper)
            : object(wrapper.get())
        {

        }

        class_type get_class() const
        {
            MonoClass* cl = mono_object_get_class(m_object);
            return class_type(cl);
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
                MonoClassField* field = get_class().get_field(field_name);
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
            return field_wrapper(m_domain, m_object, field);
        }

        MonoMethod* get_method_pointer(const char* name) const
        {
            auto it = m_method_cache.find(name);
            if (it != m_method_cache.end())
            {
                return it->second;
            }
            else
            {
                MonoMethod* method_type = get_class().get_method(name);
                if (method_type == nullptr)
                {
                    throw_exception("could not find method in class");
                }
                m_method_cache[name] = method_type;
                return method_type;
            }
        }

        template<typename FunctionSignature>
        auto get_method(const char* method_name)
        {
            using FunctorTraits = internal_get_function_type<FunctionSignature>;
            auto method_type = get_method_pointer(method_name);
            auto functor = [f = method(m_domain, method_type), o = m_object](auto&&... args) mutable -> FunctorTraits::result_type
            {
                return f.invoke_instance<FunctionSignature>(o, std::forward<decltype(args)>(args)...);
            };
            return FunctorTraits::type(std::move(functor));
        }

        template<typename T>
        T as() const
        {
            return from_mono_converter<T>::convert(m_domain, m_object);
        }

        std::string to_string() const
        {
            return as<std::string>();
        }
    };

    std::string to_string(const object& obj)
    {
        return obj.to_string();
    }
}
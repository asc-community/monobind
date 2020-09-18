#include <monobind/domain.h>
#include <monobind/method_view.h>
#include <monobind/field_view.h>

namespace monobind
{
    class class_type
    {
    public:
        class static_field_wrapper
        {
            MonoDomain* m_domain = nullptr;
            MonoVTable* m_vtable = nullptr;
            MonoClass* m_parent = nullptr;
            MonoClassField* m_field = nullptr;
            
            MonoObject* get_underlying_object_no_static_constructor_check() const
            {
                MonoObject* result = nullptr;
                class_type field_type(mono_field_get_type(m_field));
                if (mono_class_is_valuetype(field_type.get_pointer()))
                {
                    std::aligned_storage<1024> data;
                    mono_field_static_get_value(m_vtable, m_field, (void*)&data);
                    result = mono_value_box(m_domain, m_parent, (void*)&data);
                }
                else
                {
                    mono_field_static_get_value(m_vtable, m_field, (void*)&result);
                }
                return result;
            }

            MonoObject* get_underlying_object() const
            {
                auto result = get_underlying_object_no_static_constructor_check();
                if (result == nullptr)
                {
                    invoke_static_constructor(m_parent);
                    result = get_underlying_object_no_static_constructor_check();
                }
                return result;
            }

        public:
            static_field_wrapper(MonoDomain* domain, MonoClass* parent, MonoClassField* field)
                : m_domain(domain), m_parent(parent), m_field(field)
            {
                m_vtable = mono_class_vtable(m_domain, m_parent);
            }

            template<typename T>
            void set(T&& value)
            {
                auto obj = to_mono_converter<T>::convert(m_domain, std::forward<T>(value));
                mono_field_static_set_value(m_parent, m_field, (void*)obj);
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

    private:
        MonoClass* m_class = nullptr;

        static void invoke_static_constructor(MonoClass* cl)
        {
            MonoMethod* m = mono_class_get_method_from_name(cl, ".cctor", 0);
            mono_runtime_invoke(m, nullptr, nullptr, nullptr);
        }

    public:

        class_type() = default;

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

        MonoMethod* get_method_pointer(const char* name) const
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

        template<typename FunctionSignature>
        auto get_method(const char* method_name)
        {
            using FunctorTraits = internal_get_function_type<FunctionSignature>;
            auto method_type = get_method_pointer(method_name);
            auto functor = [f = method(m_domain, method_type), o = m_object](auto&&... args) mutable->FunctorTraits::result_type
            {
                return f.invoke_static<FunctionSignature>(o, std::forward<decltype(args)>(args)...);
            };
            return FunctorTraits::type(std::move(functor));
        }

        static_field_wrapper operator[](const char* field_name) const
        {
            auto field = get_field(field_name);
            return static_field_wrapper(get_current_domain(), m_class, field);
        }

        method_view get_methods() const
        {
            return method_view(get_current_domain(), m_class);
        }

        field_view get_fields() const
        {
            return field_view(m_class);
        }

        const char* get_namespace() const
        {
            return mono_class_get_namespace(m_class);
        }

        class_type get_nesting_type() const
        {
            return class_type(mono_class_get_nesting_type(m_class));
        }

        const char* get_name() const
        {
            return mono_class_get_name(m_class);
        }

        std::string to_string() const
        {
            return get_name();
        }
    };

    std::string to_string(const class_type& cl)
    {
        return cl.to_string();
    }
}
#include <monobind/domain.h>
#include <monobind/method_view.h>
#include <monobind/field_view.h>
#include <monobind/property_view.h>

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
                    std::aligned_storage_t<512> data;
                    mono_field_static_get_value(m_vtable, m_field, (void*)&data);
                    result = mono_value_box(m_domain, field_type.get_pointer(), (void*)&data);
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

            template<typename T = object>
            T get()
            {
                return as<T>();
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
            if (m_class == nullptr)
            {
                throw_exception("could not find class in image");
            }
        }

        class_type(MonoClass* cl)
            : m_class(cl)
        {

        }

        class_type(MonoType* type)
            : m_class(mono_class_from_mono_type(type))
        {
            
        }

        MonoClass* get_pointer() const
        {
            return m_class;
        }

        MonoClassField* get_field_pointer(const char* field_name) const
        {
            MONOBIND_ASSERT(m_class != nullptr);
            MonoClassField* field = mono_class_get_field_from_name(m_class, field_name);
            if (field == nullptr)
            {
                throw_exception("could not find field in class");
            }
            return field;
        }

        bool has_field(const char* field_name) const
        {
            MONOBIND_ASSERT(m_class != nullptr);
            MonoClassField* field = mono_class_get_field_from_name(m_class, field_name);
            return field != nullptr;
        }

        MonoProperty* get_property_pointer(const char* property_name) const
        {
            MONOBIND_ASSERT(m_class != nullptr);
            MonoProperty* prop = mono_class_get_property_from_name(m_class, property_name);
            if (prop == nullptr)
            {
                throw_exception("could not find property in class");
            }
            return prop;
        }

        bool has_property(const char* property_name) const
        {
            MONOBIND_ASSERT(m_class != nullptr);
            MonoProperty* prop = mono_class_get_property_from_name(m_class, property_name);
            return prop != nullptr;
        }

        MonoMethod* get_method_pointer(const char* name) const
        {
            MONOBIND_ASSERT(m_class != nullptr);
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

        bool has_method(const char* name) const
        {
            MONOBIND_ASSERT(m_class != nullptr);
            auto desc = mono_method_desc_new(name, false);
            if (desc == nullptr)
            {
                throw_exception("invalid method signature");
            }
            MonoMethod* m = mono_method_desc_search_in_class(desc, m_class);
            return m != nullptr;
        }

        template<typename T>
        void set_property(const char* name, const T& value) const
        {
            MonoProperty* prop = get_property_pointer(name);
            MonoObject* exc = nullptr;
            std::array<void*, 1> params{ (void*)to_mono_converter<T>::convert(get_current_domain(), value) };

            mono_property_set_value(prop, nullptr, params.data(), &exc);
            if (exc != nullptr)
            {
                throw_exception("exception occured while setting property value");
            }
        }

        template<typename T = object>
        T get_property(const char* name) const
        {
            MonoProperty* prop = get_property_pointer(name);
            MonoObject* exc = nullptr;

            MonoObject* result = mono_property_get_value(prop, nullptr, nullptr, &exc);
            if (exc != nullptr)
            {
                throw_exception("excpetion occured while getting property value");
            }
            return object(result).as<T>();
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
            auto field = get_field_pointer(field_name);
            return static_field_wrapper(get_current_domain(), m_class, field);
        }

        method_view get_methods() const
        {
            MONOBIND_ASSERT(m_class != nullptr);
            return method_view(get_current_domain(), m_class);
        }

        field_view get_fields() const
        {
            MONOBIND_ASSERT(m_class != nullptr);
            return field_view(m_class);
        }

        property_view get_properties() const
        {
            MONOBIND_ASSERT(m_class != nullptr);
            return property_view(m_class);
        }

        const char* get_namespace() const
        {
            MONOBIND_ASSERT(m_class != nullptr);
            return mono_class_get_namespace(m_class);
        }

        class_type get_nesting_type() const
        {
            MONOBIND_ASSERT(m_class != nullptr);
            return class_type(mono_class_get_nesting_type(m_class));
        }

        const char* get_name() const
        {
            MONOBIND_ASSERT(m_class != nullptr);
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
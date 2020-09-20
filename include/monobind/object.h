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

#include <monobind/method.h>
#include <monobind/class_type.h>

#include <string>

namespace monobind
{
    class object
    {
        MonoObject* m_object = nullptr;
        MonoDomain* m_domain = nullptr;
        uint32_t m_gchandle = 0;
        
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
            MONOBIND_ASSERT(m_domain != nullptr);
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
            MONOBIND_ASSERT(m_object != nullptr);
            MonoClass* cl = mono_object_get_class(m_object);
            return class_type(cl);
        }

        MonoObject* get_pointer() const
        {
            return m_object;
        }

        size_t get_gc_generation() const
        {
            return (size_t)mono_gc_get_generation(m_object);
        }

        void lock()
        {
            MONOBIND_ASSERT(m_gchandle == 0);
            m_gchandle = mono_gchandle_new(m_object, true);
        }

        void unlock()
        {
            MONOBIND_ASSERT(m_gchandle != 0);
            mono_gchandle_free(m_gchandle);
            m_gchandle = 0;
        }

        MonoClassField* get_field_pointer(const char* field_name) const
        {
            return get_class().get_field_pointer(field_name);
        }

        MonoProperty* get_property_pointer(const char* property_name) const
        {
            return get_class().get_property_pointer(property_name);
        }

        MonoMethod* get_method_pointer(const char* name) const
        {
            return get_class().get_method_pointer(name);
        }

        bool has_field(const char* field_name) const
        {
            return get_class().has_field(field_name);
        }

        bool has_property(const char* property_name) const
        {
            return get_class().has_property(property_name);
        }

        bool has_method(const char* name) const
        {
            return get_class().has_method(name);
        }

        field_wrapper operator[](const char* field_name) const
        {
            MonoClassField* field = get_field_pointer(field_name);
            return field_wrapper(m_domain, m_object, field);
        }

        template<typename T>
        void set_property(const char* name, const T& value) const
        {
            MonoProperty* prop = get_property_pointer(name);
            MonoObject* exc = nullptr;
            std::array<void*, 1> params { (void*)to_mono_converter<T>::convert(m_domain, value) };

            mono_property_set_value(prop, m_object, params.data(), &exc);
            if (exc != nullptr)
            {
                throw_exception("exception occured while setting property value");
            }
        }

        object get_property(const char* name) const
        {
            MonoProperty* prop = get_property_pointer(name);
            MonoObject* exc = nullptr;

            MonoObject* result = mono_property_get_value(prop, m_object, nullptr, &exc);
            if (exc != nullptr)
            {
                throw_exception("excpetion occured while getting property value");
            }
            return object(result);
        }

        template<typename T>
        T get_property(const char* name) const
        {
            return get_property(name).as<T>();
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

        template<typename FunctionSignature>
        auto get_static_method(const char* method_name)
        {
            using FunctorTraits = internal_get_function_type<FunctionSignature>;
            auto method_type = get_method_pointer(method_name);
            auto functor = [f = method(m_domain, method_type)](auto&&... args) mutable->FunctorTraits::result_type
            {
                return f.invoke_static<FunctionSignature>(std::forward<decltype(args)>(args)...);
            };
            return FunctorTraits::type(std::move(functor));
        }

        template<typename T>
        T as() const
        {
            MONOBIND_ASSERT(m_object != nullptr);
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
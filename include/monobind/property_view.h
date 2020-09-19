#include <monobind/mono_api_include.h>

namespace monobind
{
    class property_view
    {
        MonoClass* m_class;

    public:
        class property_iterator
        {
            void* m_iter = nullptr;
            MonoClass* m_class = nullptr;
            MonoProperty* m_property = nullptr;
        public:
            property_iterator() = default;

            property_iterator(MonoClass* cl, void* iter)
                : m_iter(iter), m_class(cl)
            {
                m_property = mono_class_get_properties(m_class, &m_iter);
            }

            const char* operator*() const
            {
                return mono_property_get_name(m_property);
            }

            bool operator==(const property_iterator& it) const
            {
                return m_property == it.m_property;
            }

            bool operator!=(const property_iterator& it) const
            {
                return !(*this == it);
            }

            property_iterator& operator++()
            {
                m_property = mono_class_get_properties(m_class, &m_iter);
                return *this;
            }

            property_iterator operator++(int)
            {
                auto copy = *this;
                ++(*this);
                return copy;
            }
        };

        property_view(MonoClass* cl)
            : m_class(cl)
        {

        }

        auto begin() const
        {
            return property_iterator(m_class, nullptr);
        }

        auto end() const
        {
            return property_iterator();
        }

        size_t size() const
        {
            return (size_t)mono_class_num_properties(m_class);
        }
    };
}
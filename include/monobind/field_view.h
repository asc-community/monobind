#include <monobind/method.h>

namespace monobind
{
    class field_view
    {
        MonoClass* m_class;

    public:
        class field_iterator
        {
            void* m_iter = nullptr;
            MonoClass* m_class = nullptr;
            MonoClassField* m_field = nullptr;
        public:
            field_iterator() = default;

            field_iterator(MonoClass* cl, void* iter)
                : m_iter(iter), m_class(cl)
            {
                m_field = mono_class_get_fields(m_class, &m_iter);
            }

            const char* operator*() const
            {
                return mono_field_get_name(m_field);
            }

            bool operator==(const field_iterator& it) const
            {
                return m_field == it.m_field;
            }

            bool operator!=(const field_iterator& it) const
            {
                return !(*this == it);
            }

            field_iterator& operator++()
            {
                m_field = mono_class_get_fields(m_class, &m_iter);
                return *this;
            }

            field_iterator operator++(int)
            {
                auto copy = *this;
                ++(*this);
                return copy;
            }
        };
       
        field_view(MonoClass* cl)
            : m_class(cl)
        {

        }

        auto begin() const
        {
            return field_iterator(m_class, nullptr);
        }

        auto end() const
        {
            return field_iterator();
        }

        size_t size() const
        {
            return (size_t)mono_class_num_fields(m_class);
        }
    };
}
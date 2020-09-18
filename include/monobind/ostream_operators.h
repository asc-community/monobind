#pragma once

#include <monobind/object.h>

#include <ostream>

namespace monobind
{
    std::ostream& operator<<(std::ostream& out, const object& obj)
    {
        out << obj.to_string();
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const object::field_wrapper& wrapper)
    {
        out << wrapper.get();
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const class_type::static_field_wrapper& wrapper)
    {
        out << wrapper.as<object>();
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const method& m)
    {
        out << m.get_signature();
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const class_type& cl)
    {
        out << cl.get_name();
        return out;
    }
}
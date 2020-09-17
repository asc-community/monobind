#pragma once

#include <monobind/mono_api_include.h>

namespace monobind
{
    inline MonoDomain* get_current_domain()
    {
        return mono_domain_get();
    }
}
#include <monobind/mono_api_include.h>

namespace monobind
{
    class gc
    {
    public:
        static void collect(size_t generation)
        {
            mono_gc_collect((int)generation);
        }

        static size_t get_collection_count(size_t generation)
        {
            return (size_t)mono_gc_collection_count((int)generation);
        }

        static size_t get_max_generation()
        {
            return (size_t)mono_gc_max_generation();
        }

        static size_t get_total_heap_size()
        {
            return (size_t)mono_gc_get_heap_size();
        }

        static size_t get_used_heap_size()
        {
            return (size_t)mono_gc_get_used_size();
        }
    };
}
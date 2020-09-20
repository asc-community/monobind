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
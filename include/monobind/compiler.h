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

#include <string>

namespace monobind
{
    template<typename T, typename... Args>
    void internal_append_to_command(T&& command) { }

    template<typename T, typename U, typename... Args>
    void internal_append_to_command(T&& command, U&& to_append, Args&&... other)
    {
        command += " \"";
        command += to_append;
        command += '\"';
        internal_append_to_command(std::forward<T>(command), std::forward<Args>(other)...);
    }

    class compiler
    {
        std::string m_compiler_path;

    public:
        compiler(const char* mono_root_dir)
            : m_compiler_path('\"' + std::string(mono_root_dir) + "/bin/mcs\"")
        {
            
        }

        compiler(const std::string& mono_root_dir)
            : m_compiler_path('\"' + mono_root_dir + "/bin/mcs\"")
        {

        }

        compiler(const char* mono_root_dir, const char* mono_compiler_path)
            : m_compiler_path(mono_compiler_path)
        {
            (void)mono_root_dir;
        }

        const std::string& get_path() const
        {
            return m_compiler_path;
        }

        template<typename... Paths>
        void build_library(const char* library_name, Paths&&... paths)
        {
            std::string command = '\"' + m_compiler_path;
            internal_append_to_command(command, std::forward<Paths>(paths)...);

            command += " -target:library";
            command += " -out:\"";
            command += library_name;
            command += "\"\"";

            std::system(command.c_str());
        }
    };
}
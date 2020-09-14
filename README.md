# monobind
monobind is a C lightweight header-only library that exposes C++ types in C# and vice versa, mainly yo create C# bindings of existing C++ code. It is inspired by the excellent library [Boost.Python](https://www.boost.org/doc/libs/1_74_0/libs/python) and tries to achieve simular goals of minimizing boilerplate code to implement interoperability between two languages.

### Dependencies
monobind requires at least C++11 compatible compiler to run. It only depends on [mono](https://www.mono-project.com/) - cross-platform .NET framework. You do not have to build it - simplify install it from official website on your system

### Building
You can install monobind using Cmake. First of all, you should add the library to your project by executing git command: `git submodule add https://github.com/MomoDeve/monobind`. Then simply paste the following code into your CMakeLists.txt, replacing the names & paths if necessary:
```CMake
// add monobind as subdirectory
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/monobind)

// add monobind & mono include directories
target_include_directories(current_target PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/submodules/monobind/include ${MONO_INCLUDE_DIR})
// add mono libraries
target_link_libraries(current_target ${MONO_LIBRARIES})
// add macro definition to do not hard-code path to mono directory
target_compile_definitions(current_target PUBLIC MONOBIND_MONO_ROOT="${MONO_ROOT_DIR}")
```
`FindMono.cmake` should find mono installation library. If it fails and you have mono properly installed, I will be glad to see your PR with fixing changes

### Usage
Firstly I should point out that monobind is still in develop, so you may find many features missing. I am trying my best to make the library more convinient and waiting for your suggestions. For now here is a baseline of what you can do using this library:

*Consider having .cs and .cpp files with following methods:*
```cs
using System;
using System.Runtime.CompilerServices;

namespace MonoBindExamples
{
    public class SimpleFunctionCall
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void HelloFromCpp();

        public static void HelloFromCSharp()
        {
            Console.WriteLine("C#: Hello!");

            HelloFromCpp();
        }
    }
}
```
```cpp
#include <iostream>

void hello_from_cpp()
{
    std::cout << "C++: Hello!" << std::endl;
}
```
In C# code we create a class with two static methods. One of them is defined in .cs file and calls the other, which is marked as `extern` and should be forwarded to out cpp function. To do this, we will embed mono into our C++ executable and create a dynamic library from our .cs file to link them together. To begin with, let's initialize mono:
```cpp
// if you this defined macro in CMake, it should be equal to path to the mono root directory
const char* path_to_mono = MONOBIND_MONO_ROOT;
monobind::mono mono(path_to_mono);
mono.init_jit("HelloWorldApplication");
```
With mono now we can compile our .cs file into dynamic library and load it to the executable. `monobind::compiler` accepts path to mono root or mcs compiler, with which we can create the library. Also, assembly can be loaded by passing mono domain and library name:
```cpp
    // build csharp library
    monobind::compiler compiler(mono.get_root_dir());
    compiler.build_library("SimpleFunctionCall.dll", "SimpleFunctionCall.cs");

    // load assembly
    monobind::assembly assembly(mono.get_domain(), "SimpleFunctionCall.dll");
```
And now we can finally resolve method by passing it cpp implementation as function pointer to mono. Invoking method is not that hard to - simply get the method by its signature and call it (you can also pass primitive types, aligned structures and C/C++ strings!):
```cpp
// resolve external method in C# code
mono.add_internal_call("MonoBindExamples.SimpleFunctionCall::HelloFromCpp()", hello_from_cpp);

// call C# method
monobind::method method = assembly.get_method("MonoBindExamples.SimpleFunctionCall::HelloFromCSharp()");
method.invoke();
```
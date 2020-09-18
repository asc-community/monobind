# monobind
monobind is a lightweight header-only library that exposes C++ types in C# and vice versa, mainly to create C# bindings of existing C++ code. It is inspired by the excellent library [Boost.Python](https://www.boost.org/doc/libs/1_74_0/libs/python) and tries to achieve simular goals of minimizing boilerplate code when implementing interoperability between C++ and C#.

## Dependencies
monobind requires at least C++14 compatible compiler to run. It only depends on [mono](https://www.mono-project.com/) - cross-platform .NET framework. You do not have to build it - simply install it from the official website to your system.

## Building
You can install monobind using Cmake. First of all, you should add the library to your project by executing the following git command: `git submodule add https://github.com/MomoDeve/monobind`. Then simply paste the code below into your `CMakeLists.txt`, replacing names & paths if necessary:
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

## Examples

Firstly I should point out that monobind is still in development, so you may find many features missing. I am trying my best to make the library more convinient and waiting for your suggestions (or PRs). Also, if you find the following examples not enough to understand how to use the library, consider looking through some code samples in /examples folder.

### Baseline
Here is the minimal code you have to write to call C# method from C++ and vice versa. Not arguments are passed between, so it requires the least amount of effort.

*Consider having .cs and .cpp files with the following methods:*
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
```cs
#include <iostream>

void hello_from_cpp()
{
    std::cout << "C++: Hello!" << std::endl;
}
```
In C# code we create a class with two static methods. One of them is defined in .cs file and calls the other, which is marked with `extern` and should point to our cpp function. To achieve this, we will embed mono runtime into our C++ executable, create a dynamic library from our .cs file and link them together. To begin with, let's initialize mono:
```cs
// if you defined this macro in CMake, it should be equal to the path to the mono root directory
const char* path_to_mono = MONOBIND_MONO_ROOT;
monobind::mono mono(path_to_mono);
mono.init_jit("HelloWorldApplication");
```
With mono we can now compile our .cs file into a dynamic library and load it into the executable. We can use `monobind::compiler` which accepts path to mono root or mcs compiler. After that assembly can be loaded by passing mono domain and the library name:
```cs
    // build csharp library
    monobind::compiler compiler(mono.get_root_dir());
    compiler.build_library("SimpleFunctionCall.dll", "SimpleFunctionCall.cs");

    // load assembly
    monobind::assembly assembly(mono.get_domain(), "SimpleFunctionCall.dll");
```
And now we can finally resolve method by passing its cpp implementation as callable object to mono. Invoking method is not that hard too - simply get the method by its signature and call it (you can also pass primitive types, aligned structures, C/C++ strings and arrays between C++ and C# with zero additional code!):
```cs
// resolve external method in C# code
mono.add_internal_call<void()>("MonoBindExamples.SimpleFunctionCall::HelloFromCpp()", MONOBIND_CALLABLE(hello_from_cpp));

// call C# method
monobind::method method = assembly.get_method("MonoBindExamples.SimpleFunctionCall::HelloFromCSharp()");
method.invoke_static<void()>();
```

### Assemblies, classes, objects
To interact with C# classes in most cases you need assemblies. In monobind they are represented by `monobind::assembly` and can be loaded from dynamic library files. If one library depends on another, it will automatically try to load it. Usually you will write something like this:
```cs
// get domain from initialized mono runtime object
monobind::assembly my_lib(mono.get_domain(), "my_lib.dll");
// or the following, if you do not have reference to mono object:
monobind::assembly my_lib(monobind::get_current_domain(), "my_lib.dll");
```
From assembly you can get classes and methods. Classes are represented by `monobind::class_type` and methods are represented by `monobind::method`. To retrieve them, you have to know their signature (namespace and full name for classes, full signature for methods):
```cs
// getting C# class with signature Class.Namespace.ClassName 
monobind::class_type my_class(assembly.get_image(), "Class.Namespace", "ClassName");
// getting method handle with signature Class.Namespace.ClassName.YourMethod(int), can be either static or instanced
monobind::method m = assembly.get_method("Class.Namespace.ClassName::YourMethod(int,single)");
```
And with them it is possible to create objects. They are represented by `monobind::object` class and can be easily passed around or accessed:
```cs
// if object has no constructor, simply pass domain and its class
monobind::object my_obj(mono.get_domain(), my_class);
// or specify constructor to call (two arguments (int and float) in this case):
monobind::object my_obj(mono.get_domain(), my_class, "::.ctor(int,single)", 3, 2.5f);
```
You can access fields, call methods or pass them as method arguments:
```cs
// call static method with object of some type as first argument and int as second argument:
my_method.invoke_static<void(monobind::object, int>(my_obj, 3);
// call instanced method with object as this pointer and int as argument:
my_method.invoke_instance<void(int)>(my_obj, 3);
```
```cs
// same as above, but my_obj is passed implicitly
my_obj.get_method<void(int)>("::SomeMethod(int)")(3);
// and finally, if you need static method but have only object instance:
my_obj.get_static_method<void(int)>("::SomeStaticMethod(int)")(3);
```
```cs
// you can also access fields
monobind::object x = my_obj["someField"];
my_obj["someField"] = 3;

// or explicitly cast field to type:
auto x = my_obj["someField"].as<int>();
```
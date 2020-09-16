#include <monobind/monobind.h>

#include <iostream>

void hello_from_cpp()
{
    std::cout << "C++: Hello!" << std::endl;
}

int main()
{
    // create mono and initialize runtime
    const char* path_to_mono = MONOBIND_MONO_ROOT;
    monobind::mono mono(path_to_mono);
    mono.init_jit("SimpleFunctionCallApp");

    // resolve external method in C# code
    mono.add_internal_call<void(*)()>("MonoBindExamples.SimpleFunctionCall::HelloFromCpp()", MONOBIND_CALLABLE(hello_from_cpp));

    // build csharp library
    monobind::compiler compiler(mono.get_root_dir());
    compiler.build_library("SimpleFunctionCall.dll", "SimpleFunctionCall.cs");

    // load assembly and get C# method
    monobind::assembly assembly(mono.get_domain(), "SimpleFunctionCall.dll");
    monobind::method method = assembly.get_method("MonoBindExamples.SimpleFunctionCall::HelloFromCSharp()");

    // call C# method
    method.invoke_static<void>();

    return 0;
}
#include <monobind/monobind.h>

#include <iostream>

int main()
{
    const char* path_to_mono = MONOBIND_MONO_ROOT;
    monobind::mono mono(path_to_mono);
    mono.init_jit("VirtualMethods");

    // build csharp library
    monobind::compiler compiler(mono.get_root_dir());
    compiler.build_library("VirtualMethods.dll", "VirtualMethods.cs");
    
    // load assembly and get C# method
    monobind::assembly assembly(mono.get_domain(), "VirtualMethods.dll");

    monobind::object base_instance = assembly.get_method("MonoBindExamples.Factory::CreateBase()").as_function<monobind::object()>()();
    monobind::object derived1_instance = assembly.get_method("MonoBindExamples.Factory::CreateDerived1()").as_function<monobind::object()>()();
    monobind::object derived2_instance = assembly.get_method("MonoBindExamples.Factory::CreateDerived2()").as_function<monobind::object()>()();
    
    std::cout << "instances:" << std::endl;
    std::cout << base_instance << std::endl;
    std::cout << derived1_instance << std::endl;
    std::cout << derived2_instance << std::endl;
    std::cout  << std::endl;

    std::cout << "method calls:" << std::endl;
    base_instance.get_method<void()>("::Foo()")();
    derived1_instance.get_method<void()>("::Foo()")();
    derived2_instance.get_method<void()>("::Foo()")();

    return 0;
}
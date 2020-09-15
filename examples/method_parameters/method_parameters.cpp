#include <monobind/monobind.h>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    const char* path_to_mono = MONOBIND_MONO_ROOT;
    monobind::mono mono(path_to_mono);
    mono.init_jit("MethodParametersApp");

    monobind::compiler compiler(mono.get_root_dir());
    compiler.build_library("MethodParameters.dll", "MethodParameters.cs");

    // load assembly and get C# method
    monobind::assembly assembly(mono.get_domain(), "MethodParameters.dll");

    monobind::method method = assembly.get_method("MonoBindExamples.MethodParameters::BeautifyTime(single)");
    auto beautify_time = method.as_function<std::string(*)(float)>();
    std::cout << "2.5f    - " << beautify_time(2.5f)    << std::endl;
    std::cout << "0.5f    - " << beautify_time(0.5f)    << std::endl;
    std::cout << "0.0003f - " << beautify_time(0.0003f) << std::endl;

    std::cout << std::endl;

    auto printer = assembly.get_method("MonoBindExamples.MethodParameters::PrintPrimitives(char,int16,int,long,single,double)");
    printer.invoke(L'X', 256, 123456789, 9999999999999, 2.3f, 0.33333333333333);

    std::cout << std::endl;

    auto split_method = assembly.get_method("MonoBindExamples.MethodParameters::SplitString(string)");
    auto split_string = split_method.as_function<std::vector<std::string>(*)(std::string)>();

    auto concat_method = assembly.get_method("MonoBindExamples.MethodParameters::ConcatString(string[])");
    auto concat_string = concat_method.as_function<std::string(*)(std::vector<std::string>)>();

    auto splitted = split_string("hello world from c++!");   
    auto concated = concat_string(splitted);
    
    std::cout << concated << std::endl;

    return 0;
}
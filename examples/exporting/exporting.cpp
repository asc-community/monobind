#include <monobind/monobind.h>

#include <iostream>
#include <fstream>

struct vec3
{
    float x;
    float y;
    float z;

    float length;

    vec3 normalized() const
    {
        auto length = std::sqrt(x * x + y * y + z * z);
        return vec3{ x / length, y / length, z / length, 1.0f };
    }
};

class widget
{
    std::string name;
public:
    vec3 inner;

    int width;
    int height;
    int size;
    
    const std::string& get_name() const { return name; }
    void set_name(const std::string& n) { name = n; }

    static std::string get_class_name() { return "widget"; }
};


int main()
{
    monobind::mono mono("C:/Program Files/Mono");
    mono.init_jit("ExportingExample");

    const char* codegen_filepath = "D:/repos/monobind/examples/exporting/CodeGen.cs";
    const char* sample_filepath = "D:/repos/monobind/examples/exporting/SampleFile.cs";
    std::ofstream codegen(codegen_filepath);

    monobind::code_generator gen(mono, codegen);

    gen.add_type<vec3>("Vector3");

    monobind::export_class<widget>("Widget", gen)
        .field("Width", &widget::width)
        .field("Height", &widget::height)
        .readonly_field("Size", &widget::size)
        .readonly_field("Inner", &widget::inner)
        .property("Name", MONOBIND_GET(widget, get_name), MONOBIND_SET(widget, set_name))
        .static_method<std::string()>("GetClassName", MONOBIND_STATIC_METHOD(widget, get_class_name))
        ;

    monobind::export_struct<vec3>("Vector3", gen)
        .field("x", &vec3::x)
        .field("y", &vec3::y)
        .field("z", &vec3::z)
        .readonly_field("Length", &vec3::length)
        .property("Normalized", MONOBIND_GET(vec3, normalized))
        ;


    codegen.close();

    monobind::compiler compiler(mono.get_root_dir());
    compiler.build_library("CodeGen.dll", codegen_filepath, sample_filepath);

    mono.add_internal_call<widget()>("Program::GetWidget()", []
        {
            widget w;
            w.width = 10;
            return w;
        });

    monobind::assembly assembly(mono.get_domain(), "CodeGen.dll");
    auto main_method = assembly.get_method("Program::Main()").as_function<void()>();
    main_method();

    return 0;
}
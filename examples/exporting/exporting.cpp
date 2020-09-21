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
};


int main()
{
    monobind::mono mono(MONOBIND_MONO_ROOT);
    mono.init_jit("ExportingExample");

    std::ofstream codegen("D:/repos/monobind/examples/exporting/CodeGen.cs");

    monobind::code_generator gen(mono, codegen);

    gen.add_type<vec3>("Vector3");

    monobind::export_class<widget>("Widget", gen)
        .field("Width", &widget::width)
        .field("Height", &widget::height)
        .readonly_field("Size", &widget::size)
        .readonly_field("Inner", &widget::inner)
        .property("Name", MONOBIND_GET(widget, get_name), MONOBIND_SET(widget, set_name))
        ;

    monobind::export_struct<vec3>("Vector3", gen)
        .field("x", &vec3::x)
        .field("y", &vec3::y)
        .field("z", &vec3::z)
        .readonly_field("Length", &vec3::length)
        .property("Normalized", MONOBIND_GET(vec3, normalized))
        ;

    return 0;
}
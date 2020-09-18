#include <monobind/monobind.h>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    monobind::mono mono(MONOBIND_MONO_ROOT);
    mono.init_jit("AngouriMathExample");

    // load AngouriMath assembly
    monobind::assembly assembly(mono.get_domain(), "AngouriMath.dll");

    monobind::class_type real_number_class(assembly.get_image(), "AngouriMath", "Entity/Number/Real");
    std::cout << real_number_class << ".PositiveInfinity = " << real_number_class["PositiveInfinity"] << std::endl;

    auto from_string = assembly.get_method("Entity::op_Implicit(string)").as_function<monobind::object(std::string)>();
    auto expr = from_string("x2 - 3x + 2");

    auto solve = assembly.get_method("MathS::SolveEquation(Entity,Entity/Variable)").as_function<monobind::object(monobind::object, monobind::object)>();
    auto to_variable = assembly.get_method("Entity/Variable::op_Implicit(string)").as_function<monobind::object(std::string)>();
    auto var = to_variable("x");
    auto result = solve(expr, var); 
    std::cout << "solutions of (" << expr << "): " <<  result << std::endl;

    return 0;
}
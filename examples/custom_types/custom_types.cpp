#include <monobind/monobind.h>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    const char* path_to_mono = MONOBIND_MONO_ROOT;
    monobind::mono mono(path_to_mono);
    mono.init_jit("CustomTypesApp");

    monobind::compiler compiler(mono.get_root_dir());
    compiler.build_library("CustomTypes.dll", "CustomTypes.cs");

    // load assembly and get C# method
    monobind::assembly assembly(mono.get_domain(), "CustomTypes.dll");

    monobind::class_type player_class(assembly.get_image(), "MonoBindExamples", "Player");
    monobind::object player_object(mono.get_domain(), player_class, "::.ctor(string,int)", "Bob", 100);

    monobind::class_type monster_class(assembly.get_image(), "MonoBindExamples", "Monster");
    monobind::object monster_object(mono.get_domain(), monster_class, "::.ctor(string,int,int)", "Dragon", 1000, 25);

    auto player_to_string = player_object.get_method<std::string()>("::ToString()");
    std::cout << player_to_string() << std::endl;

    mono.add_internal_call<void(monobind::object, monobind::object)>("MonoBindExamples.Monster::Attack(MonoBindExamples.Player)",
        [](monobind::object monster, monobind::object player)
        {
            std::string player_name = player["_name"];
            int player_health = player["_health"];

            std::string monster_name = monster["_name"];
            int monster_damage = monster["_damage"];

            player["_health"] = player_health - monster_damage;

            std::cout << monster_name << " damaged " << player_name << " for " << monster_damage << " health" << std::endl;
        }
    );

    monster_object.get_method<void(monobind::object)>("::Attack(Player)")(player_object);

    std::cout << player_to_string() << std::endl;

    return 0;
}
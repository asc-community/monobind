#include <monobind/monobind.h>

#include <iostream>

struct Cat
{
	int x;
	char c;
	float d;
};

void cat_mew_func(monobind::object cat)
{
	Cat cat_struct;
	cat_struct.x = cat["x"];
	cat_struct.c = cat["c"];
	cat_struct.d = cat["d"];
	std::cout << "mew from cat: " << "x: " << cat_struct.x << " c: " << cat_struct.c << " d: " << cat_struct.d << std::endl;
	cat["x"] = 3;
	cat["c"] = 'Z';
	cat["d"] = -1.7f;
}

int main(int argc, char* argv[])
{
	monobind::mono mono(MONOBIND_MONO_ROOT);
	mono.init_jit("SampleApplication");

	monobind::compiler compiler(mono.get_root_dir());
	compiler.build_library("Dog.dll", "Dog.cs");

	monobind::assembly assembly(mono.get_domain(), "Dog.dll");

	mono.add_internal_call<void(monobind::object)>("Cat::Mew(CatImpl)", MONOBIND_CALLABLE(cat_mew_func));

	mono.add_internal_call<std::string(std::string)>("Cat::MewMew(string)", [](std::string str)
	{
		std::cout << "mew mew: " << str << std::endl;
		return str;
	});

	auto mewmew = assembly.get_method("Cat::MewMew(string)").as_function<std::string(std::string)>();
	std::cout << "in C++: " << mewmew("hi!") << std::endl;

	monobind::method method = assembly.get_method("Dog::Type()");

	//run the method
	std::cout << "Running the static method: Dog::Type()" << std::endl;
	method.invoke_static<void>();

	//Get the class
	MonoClass* dogclass;
	dogclass = mono_class_from_name(assembly.get_image(), "", "Dog");
	if (!dogclass)
	{
		std::cout << "mono_class_from_name failed" << std::endl;
		return 1;
	}

	//Create a instance of the class
	monobind::object dogA(mono.get_domain(), dogclass);

	auto object_method = assembly.get_method("Dog::Bark(int)");

	//Run the method
	std::cout << "Running the method: Dog::Bark(int)" << std::endl;
	object_method.invoke_instance<void>(dogA, 3);

	return 0;
}
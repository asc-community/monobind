#include <monobind/monobind.h>

#include <iostream>

struct Cat
{
	int x;
	char c;
	float d;
};

void cat_mew_func(Cat cat)
{
	std::cout << "mew from cat: " << "x: " << cat.x << " c: " << cat.c << " d: " << cat.d << std::endl;
}

int main(int argc, char* argv[])
{
	monobind::mono mono(MONOBIND_MONO_ROOT);
	mono.init_jit("SampleApplication");

	monobind::compiler compiler(mono.get_root_dir());
	compiler.build_library("Dog.dll", "Dog.cs");

	monobind::assembly assembly(mono.get_domain(), "Dog.dll");

	mono.add_internal_call("Cat::Mew(CatImpl)", cat_mew_func);
	mono.add_internal_call<void(*)(MonoString*)>("Cat::MewMew(string)", [](MonoString* str)
	{
		char* s = mono_string_to_utf8(str);
		std::string cppstring(s, s + mono_string_length(str));
		mono_free(s);
		std::cout << "mew mew: " << cppstring << std::endl;
	});

	assembly.get_method("Cat::MewMew(string)").invoke("hi");

	monobind::method method = assembly.get_method("Dog::Type()");

	//run the method
	std::cout << "Running the static method: Dog::Type()" << std::endl;
	method.invoke();

	//Get the class
	MonoClass* dogclass;
	dogclass = mono_class_from_name(assembly.get_image(), "", "Dog");
	if (!dogclass)
	{
		std::cout << "mono_class_from_name failed" << std::endl;
		return 1;
	}

	//Create a instance of the class
	MonoObject* dogA;
	dogA = mono_object_new(mono.get_domain(), dogclass);
	if (!dogclass)
	{
		std::cout << "mono_object_new failed" << std::endl;
		return 1;
	}

	//Call its default constructor
	mono_runtime_object_init(dogA);

	auto object_method = assembly.get_method("Dog::Bark(int)").as_function<MonoObject*, int>();

	//Run the method
	std::cout << "Running the method: Dog::Bark(int)" << std::endl;
	object_method(dogA, 3);

	return 0;
}
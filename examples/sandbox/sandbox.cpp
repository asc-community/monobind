#include <monobind/monobind.h>

#include <iostream>

class Cat
{
public:
	static monobind::class_type cat_type;

	int x;
	char c;
	float d;

	Cat(){ }
};

monobind::class_type Cat::cat_type;

template<>
struct monobind::from_mono_converter<Cat>
{
	static Cat convert(MonoDomain* domain, MonoObject* o)
	{
		Cat c;
		auto obj = monobind::to_object(o);
		c.c = obj["c"];
		c.d = obj["d"];
		c.x = obj["x"];
		return c;
	}
};

template<>
struct monobind::to_mono_converter<Cat>
{
	static MonoObject* convert(MonoDomain* domain, const Cat& c)
	{
		monobind::object obj(domain, Cat::cat_type);
		obj["c"] = c.c;
		obj["d"] = c.d;
		obj["x"] = c.x;
		return obj.get_pointer();
	}
};

template<>
struct monobind::can_be_trivially_converted<Cat>
{
	static constexpr size_t value = false;
};

void cat_mew_func(Cat cat_struct)
{
	std::cout << "mew from cat: " << "x: " << cat_struct.x << " c: " << cat_struct.c << " d: " << cat_struct.d << std::endl;
}

int main(int argc, char* argv[])
{
	monobind::mono mono(MONOBIND_MONO_ROOT);
	mono.init_jit("SampleApplication");

	monobind::compiler compiler(mono.get_root_dir());
	compiler.build_library("Dog.dll", "Dog.cs");

	monobind::assembly assembly(mono.get_domain(), "Dog.dll");

	Cat::cat_type = monobind::class_type(assembly.get_image(), "", "CatImpl");

	mono.add_internal_call<void(Cat)>("Cat::Mew(CatImpl)", MONOBIND_CALLABLE(cat_mew_func));
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
	method.invoke_static<void()>();

	monobind::class_type dogclass(assembly.get_image(), "", "Dog");

	//Create a instance of the class
	monobind::object dog(mono.get_domain(), dogclass);

	//Run the method
	std::cout << "Running the method: Dog::Bark(int)" << std::endl;
	dog.get_method<void(int)>("::Bark(int)")(3);

	return 0;
}
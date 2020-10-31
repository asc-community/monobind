using System;
using System.Runtime.CompilerServices;

namespace MonoBindExamples
{
    public class Base
    {
        public virtual void Foo()
        {
            Console.WriteLine("Called Base::Foo");
        }
    }

    public static class Factory
    {
        public static Base CreateBase()
        {
            return new Base();
        }

        public static Base CreateDerived1()
        {
            return new Derived1();
        }

        public static Base CreateDerived2()
        {
            return new Derived2();
        }
    }

    public class Derived1 : Base
    {
        public override void Foo()
        {
            Console.WriteLine("Called Derived1::Foo");
        }
    }

    public class Derived2 : Base
    {

    }
}
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

public class CatImpl
{
    public int x;
    public char c;
    public float d;
}

public class Cat
{
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public static extern void Mew(CatImpl cat);

    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public static extern string MewMew(string s);
}

public class Dog
{
    static public void Type()
    {
        Console.WriteLine("a Dog!");

        Console.WriteLine("calling cat mew...");
        CatImpl cat = new CatImpl();
        cat.x = 1;
        cat.c = 'x';
        cat.d = 1.43f;
        Cat.Mew(cat);
        Console.WriteLine($"cat after call: x: {cat.x}, c: {cat.c}, d: {cat.d}");
    }

    public void Bark()
    {
        Console.WriteLine("bark!");
    }

    public void Bark(int times)
    {
        for (var i = 0; i < times; ++i)
            Console.WriteLine("bark!");
    }
}
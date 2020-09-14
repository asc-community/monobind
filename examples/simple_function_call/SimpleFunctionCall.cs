using System;
using System.Runtime.CompilerServices;

namespace MonoBindExamples
{
    public class SimpleFunctionCall
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void HelloFromCpp();

        public static void HelloFromCSharp()
        {
            Console.WriteLine("C#: Hello!");

            HelloFromCpp();
        }
    }
}
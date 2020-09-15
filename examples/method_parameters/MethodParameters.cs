using System;
using System.Runtime.CompilerServices;

namespace MonoBindExamples
{
    public class MethodParameters
    {
        public static string BeautifyTime(float seconds)
        {
            if(seconds >= 1.0f)
            {
                return seconds.ToString() + 's';
            }
            else if(seconds >= 0.001f)
            {
                return (seconds * 1000.0f).ToString() + "ms";
            }
            else
            {
                return (seconds * 1000000.0f).ToString() + "ns";
            }
        }

        public static void PrintPrimitives(char c, short s, int i, long l, float f, double d)
        {
            Console.WriteLine($"recieved char: {c}, short: {s}, int: {i}, long: {l}, float: {f}, double: {d}");
        }

        public static string[] SplitString(string s)
        {
            Console.WriteLine("splitting: " + s);
            return s.Split();
        }

        public static string ConcatString(string[] s)
        {
            Console.WriteLine("recieved array: " + s.ToString());
            return "{ " + string.Join(", ", s) + " }";
        }
    }
}
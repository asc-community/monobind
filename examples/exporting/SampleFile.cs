using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

class Program
{
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern IntPtr GetWidget();

    public static void Main()
    {
        Widget w = new Widget(GetWidget());
        Console.WriteLine(w.Width);
    }
}
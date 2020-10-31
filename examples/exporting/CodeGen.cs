using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

class Widget
{
	private IntPtr _nativeHandle;
	public Widget(IntPtr nativeHandle) { _nativeHandle = nativeHandle; }

	// Widget::get_Width(intptr,uint)
	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern int get_Width(IntPtr _self, uint _offset);
	// Widget::set_Width(intptr,uint,int)
	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern void set_Width(IntPtr _self, uint _offset, int _value);
	public int Width { get => get_Width(_nativeHandle, 56); set => set_Width(_nativeHandle, 56, value); }

	// Widget::get_Height(intptr,uint)
	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern int get_Height(IntPtr _self, uint _offset);
	// Widget::set_Height(intptr,uint,int)
	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern void set_Height(IntPtr _self, uint _offset, int _value);
	public int Height { get => get_Height(_nativeHandle, 60); set => set_Height(_nativeHandle, 60, value); }

	// Widget::get_Size(intptr,uint)
	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern int get_Size(IntPtr _self, uint _offset);
	public int Size => get_Size(_nativeHandle, 64);

	// Widget::get_Inner(intptr,uint)
	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern Vector3 get_Inner(IntPtr _self, uint _offset);
	public Vector3 Inner => get_Inner(_nativeHandle, 40);

	// Widget::get_Name(intptr)
	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern string get_Name(IntPtr _self);
	// Widget::set_Name(intptr,string)
	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern void set_Name(IntPtr _self, string _value);
	public string Name { get => get_Name(_nativeHandle); set => set_Name(_nativeHandle, value); }

	// Widget::GetClassName()
	[MethodImpl(MethodImplOptions.InternalCall)]
	public static extern string GetClassName();

}

[StructLayout(LayoutKind.Explicit)]
struct Vector3
{
	[FieldOffset(0)] public float x;

	[FieldOffset(4)] public float y;

	[FieldOffset(8)] public float z;

	[FieldOffset(12)] private float _Length;
	public float Length => _Length;

	// Vector3::get_Normalized(Vector3&)
	[MethodImpl(MethodImplOptions.InternalCall)]
	private static extern Vector3 get_Normalized(ref Vector3 _self);
	public Vector3 Normalized => get_Normalized(ref this); 
}


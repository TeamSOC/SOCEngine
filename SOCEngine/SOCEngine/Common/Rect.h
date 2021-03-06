#pragma once

template <typename Type>
struct Size
{
public:
	Size(Type _w, Type _h) : w(_w), h(_h) {}

	template <typename NewType>
	Size<NewType> Cast() const
	{
		return Size<NewType>(static_cast<NewType>(w), static_cast<NewType>(h));
	}

public:
	Type w, h;
};

template <typename Type>
struct Rect
{
public:
	Rect() : x(0), y(0), size(0, 0) { }
	Rect(Type _x, Type _y, Type _w, Type _h) : x(_x), y(_y), size(_w, _h) { }
	Rect(Type _x, Type _y, const Size<Type>& _size) : x(_x), y(_y), size(_size) { }

	template <typename NewType>
	Rect<NewType> Cast() const
	{
		return Rect<NewType>(	static_cast<NewType>(x),
								static_cast<NewType>(y),
								static_cast<NewType>(size.w),
								static_cast<NewType>(size.h)	);
	}

public:
	Type x, y;
	Size<Type> size;
};

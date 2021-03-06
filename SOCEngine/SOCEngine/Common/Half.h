#pragma once

#include "Common.h"

class Half final
{
public:
	explicit Half(ushort);
	explicit Half(float);
	
	GET_CONST_ACCESSOR(Value, ushort, _value);

	operator float() const;

private:
	ushort _value;
};

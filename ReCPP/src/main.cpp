#include "Type_Traits/Type_Traits.h"
#include <iostream>

int main()
{
	auto a = ACBYTES::conditional<int, const int, ACBYTES::is_const<const int>::value>::result();
	a = 6;
}
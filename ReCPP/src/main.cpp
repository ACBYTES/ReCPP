#include "Type_Traits/Type_Traits.h"
#include "Functional/Function.h"
#include <iostream>

class FunctionHolder
{
public:
	void Do() {
		std::cout << 22;
	}

	static int F() {
		std::cout << 88;
		return 5;
	}

	static int Add(int A, int B)
	{
		std::cout << A + B;
		return A + B + 1000;
	}
};

int main()
{
	ACBYTES::Function<int, void, int, int> a = ACBYTES::Function<int, void, int, int>(FunctionHolder::Add);
	auto u = a(5, 10);
}
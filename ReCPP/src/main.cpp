#include "Type_Traits/Type_Traits.h"
#include "Functional/Function.h"
#include <iostream>
#include <typeinfo>
#include <vector>
#include <utility>

class FunctionHolder
{
public:

	std::string s;

	FunctionHolder() {
		std::cout << "Instantiated\n";
		s = "LOL";
	}

	void Do() {
		std::cout << s;
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

static void SS()
{
	
}

int main()
{
	auto a = new FunctionHolder();
	ACBYTES::Function::WrapFunction<void, FunctionHolder>(a, &FunctionHolder::Do)();
	//ACBYTES::Function::WrapFunction<void, FunctionHolder>(a, &FunctionHolder::Do);
	//auto a = ACBYTES::Function::WrapFunction<int>(&FunctionHolder::F);
	//auto b = ACBYTES::Function::WrapFunction<void, 
}
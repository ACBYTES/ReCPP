#pragma once
#include <utility>

namespace ACBYTES
{
	/*
	A function wrapper that can deal with static, non-static, and member functions.
	@tparam RT Return type of the function.
	@tparam Class Class that contains the function.
	@tparam ArgT Arguments that should be passed to the function.
	*/
	template <typename RT, typename Class, typename... ArgT>
	class Function
	{
		typedef RT(Class::* funcType)(ArgT...);
		Class* _class;
		funcType _funcPtr;
	public:

		Function(Class* ClassPtr, funcType FuncPtr)
		{
			_class = ClassPtr;
			_funcPtr = FuncPtr;
		}
	public:
		RT operator()(ArgT... Args)
		{
			return (_class->*_funcPtr)(std::forward<ArgT>(Args)...);
		}
	};

	/*
	A function wrapper that can deal with static, non-static, and member functions.
	@tparam RT Return type of the function.
	@tparam Class Class that contains the function.
	*/
	template <typename RT, typename Class>
	class Function<RT, Class, void>
	{
		typedef RT(Class::* funcType)(void);
		Class* _class;
		funcType _funcPtr;
	public:
		Function(Class* ClassPtr, funcType FuncPtr)
		{
			_class = ClassPtr;
			_funcPtr = FuncPtr;
		}

		RT operator()(void)
		{
			return (_class->*_funcPtr)();
		}
	};

	/*
	A function wrapper that can deal with static, non-static, and member functions.
	@tparam RT Return type of the function.
	@tparam ArgT Arguments that should be passed to the function.
	*/
	template <typename RT, typename... ArgT>
	class Function<RT, void, ArgT...>
	{
		typedef RT(*funcType)(ArgT...);
		funcType _funcPtr;
	public:

		Function(funcType FuncPtr)
		{
			_funcPtr = FuncPtr;
		}
	public:
		RT operator()(ArgT... Args)
		{
			return _funcPtr(std::forward<ArgT>(Args)...);
		}
	};

	/**
	A function wrapper that can deal with static, non-static, and member functions.
	@tparam RT Return type of the function.
	*/
	template <typename RT>
	class Function<RT, void, void>
	{
		typedef RT(*funcType)(void);
		funcType _funcPtr;
	public:
		Function(funcType FuncPtr)
		{
			_funcPtr = FuncPtr;
		}

		RT operator()(void)
		{
			return _funcPtr();
		}
	};
}
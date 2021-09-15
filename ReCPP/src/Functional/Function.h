#pragma once
#include <utility>
#include "../Memory/Smart_Pointers.h"

namespace ACBYTES
{
	class Function
	{
		/*
		A function wrapper that can deal with static, non-static, and member functions.
		@param RT Return type of the function.
		@param Class Class that contains the function.
		@param ArgT Arguments that should be passed to the function.
		*/
		template <typename RT, typename Class, typename... ArgT>
		class Func
		{
		public:
			typedef RT(Class::* funcType)(ArgT...);

		private:
			Class* _class;
			funcType _funcPtr;

		public:
			Func(Class* ClassPtr, funcType FuncPtr) : _class(ClassPtr), _funcPtr(FuncPtr)
			{
			}

			Func(Shared_Ptr<Class>&& ClassPtr, funcType FuncPtr) : _class(ClassPtr.Get()), _funcPtr(FuncPtr)
			{
			}

			RT operator()(ArgT... Args)
			{
				return (_class->*_funcPtr)(std::forward<ArgT>(Args)...);
			}

			Func() = delete;
		};

		/*
		A function wrapper that can deal with static, non-static, and member functions.
		@param RT Return type of the function.
		@param Class [Class type that contains the function].
		*/
		template <typename RT, typename Class>
		class Func<RT, Class>
		{
		public:
			typedef RT(Class::* funcType)(void);

		private:
			Class* _class;
			funcType _funcPtr;

		public:
			Func(Class* ClassPtr, funcType FuncPtr) : _class(ClassPtr), _funcPtr(FuncPtr)
			{
			}

			Func(Shared_Ptr<Class>&& ClassPtr, funcType FuncPtr) : _class(ClassPtr.Get()), _funcPtr(FuncPtr)
			{
			}

			RT operator()(void)
			{
				return (_class->*_funcPtr)();
			}

			Func() = delete;
		};

		/*
		A function wrapper that can deal with static, non-static, and member functions.
		@param RT Return type of the function.
		@param ArgT Arguments that should be passed to the function.
		*/
		template <typename RT, typename... ArgT>
		class Func<RT, void, ArgT...>
		{
		public:
			typedef RT(*funcType)(ArgT...);

		private:
			funcType _funcPtr;

		public:
			Func(funcType FuncPtr) : _funcPtr(FuncPtr)
			{
			}

			RT operator()(ArgT... Args)
			{
				return _funcPtr(std::forward<ArgT>(Args)...);
			}

			Func() = delete;
		};

		/*
		A function wrapper that can deal with static, non-static, and member functions.
		@param RT Return type of the function.
		*/
		template <typename RT>
		class Func<RT, void>
		{
		public:
			typedef RT(*funcType)(void);

		private:
			funcType _funcPtr;

		public:
			Func(funcType FuncPtr) : _funcPtr(FuncPtr)
			{
			}

			RT operator()(void)
			{
				return _funcPtr();
			}

			Func() = delete;
		};

	public:

		/*
		* Wraps non-member function in a Func class.
		* @param RT [Return Type of the Function].
		* @param ArgT [Type of the Arguments Passed to the Function].
		* @param FunctionPointer [Target Function]
		*/
		static Function::Func<void, void> WrapFunction(typename Function::Func<void, void>::funcType FunctionPointer)
		{
			return Function::Func<void, void>(FunctionPointer);
		}

		/*
		* Wraps non-member function in a Func class.
		* @param RT [Return Type of the Function].
		* @param ArgT [Type of the Arguments Passed to the Function].
		* @param FunctionPointer [Target Function]
		*/
		template<typename RT, typename... ArgT>
		static Function::Func<RT, void, ArgT...> WrapFunction(typename Function::Func<RT, void, ArgT...>::funcType FunctionPointer)
		{
			return Function::Func<RT, void, ArgT...>(FunctionPointer);
		}

		/*
		* Wraps member function in a Func class.
		* @param RT [Return Type of the Function].
		* @param Class [Containing Class Type].
		* @param ArgT [Type of the Arguments Passed to the Function].
		* @param ClassPointer [Pointer to instance of Class].
		* @param FunctionPointer [Target Function]
		*/
		template<typename RT, typename Class, typename... ArgT>
		static Function::Func<RT, Class, ArgT...> WrapFunction(Class* ClassPointer, typename Function::Func<RT, Class, ArgT...>::funcType FunctionPointer)
		{
			return Function::Func<RT, Class, ArgT...>(ClassPointer, FunctionPointer);
		}

		/*
		* Wraps member function in a Func class.
		* @param RT [Return Type of the Function].
		* @param Class [Containing Class Type].
		* @param ArgT [Type of the Arguments Passed to the Function].
		* @param ClassPointer [Shared pointer to an instance of Class].
		* @param FunctionPointer [Target Function]
		*/
		template<typename RT, typename Class, typename... ArgT>
		static Function::Func<RT, Class, ArgT...> WrapFunction(Shared_Ptr<Class> ClassPointer, typename Function::Func<RT, Class, ArgT...>::funcType FunctionPointer)
		{
			return Function::Func<RT, Class, ArgT...>(std::move(ClassPointer), FunctionPointer);
		}
	};
}
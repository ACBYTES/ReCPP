#pragma once
#include <utility>

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
			Func(Class* ClassPtr, funcType FuncPtr)
			{
				_class = ClassPtr;
				_funcPtr = FuncPtr;
			}

			RT operator()(ArgT... Args)
			{
				return (_class->*_funcPtr)(std::forward<ArgT>(Args)...);
			}

		private:
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
			Func(Class* ClassPtr, funcType FuncPtr)
			{
				_class = ClassPtr;
				_funcPtr = FuncPtr;
			}

			RT operator()(void)
			{
				return (_class->*_funcPtr)();
			}

		private:
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
			Func(funcType FuncPtr)
			{
				_funcPtr = FuncPtr;
			}

			RT operator()(ArgT... Args)
			{
				return _funcPtr(std::forward<ArgT>(Args)...);
			}

		private:
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
			Func(funcType FuncPtr)
			{
				_funcPtr = FuncPtr;
			}

			RT operator()(void)
			{
				return _funcPtr();
			}

		private:
			Func() = delete;
		};

	public:
		/*
		* Wraps non-member function in a Func class.
		* @param RT [Return Type of the Function].
		* @param ArgT [Type of the Argumetns Passed to the Function].
		* @param FunctionPointer [Target Function]
		*/
		template<typename RT, typename... ArgT>
		static Func<RT, void, ArgT...> WrapFunction(typename Func<RT, void, ArgT...>::funcType FunctionPointer)
		{
			return Func<RT, void, ArgT...>(FunctionPointer);
		}

		/*
		* Wraps member function in a Func class.
		* @param RT [Return Type of the Function].
		* @param Class [Containing Class Type].
		* @param ArgT [Type of the Argumetns Passed to the Function].
		* @param ClassPointer [Instance of Class].
		* @param FunctionPointer [Target Function]
		*/
		template<typename RT, typename Class, typename... ArgT>
		static Func<RT, Class, ArgT...> WrapFunction(Class* ClassPointer, typename Func<RT, Class, ArgT...>::funcType FunctionPointer)
		{
			return Func<RT, Class, ArgT...>(ClassPointer, FunctionPointer);
		}
	};
}
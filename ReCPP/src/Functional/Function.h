#pragma once

#include <utility>
#include "Smart_Pointers.h"
#include "Type_Traits.h"

//If enabled, function classes initialized with a shared pointer (ACBYTES::Shared_Ptr) will keep a copy of the shared pointer to avoid the function containing class from getting deleted. If initialized using the pure pointer constructor, shared pointer to the class will be initialized as an empty pointer.
#define SHARED_PTR_FUNCTIONS 0

namespace ACBYTES
{
	class Function
	{
#pragma region Func
	public:
		enum Post_Qualifiers
		{
			NONE = 0, CONST = 1, VOLATILE = 2
		};

	private:

#pragma region RT, CLASS, PQ, ARGT

		/*
		A function wrapper that can deal with static, non-static, and member functions.
		@param RT [Return type of the function].
		@param Class [Class type that contains the function].
		@param PQ [Post-qualifier for member function].
		@param ArgT [Arguments that should be passed to the function].
		*/
		template <typename RT, typename Class, Post_Qualifiers PQ = NONE, typename... ArgT>
		class Func
		{
		public:
			//typedef RT(Class::* funcType)(ArgT...);
			using funcType = RT(Class::*)(ArgT...);

		private:
			Class* _class;
			funcType _funcPtr;

#if SHARED_PTR_FUNCTIONS
			Shared_Ptr<Class> _shared_class_ptr;
#endif //SHARED_PTR_FUNCTIONS

		public:
			Func(Class* ClassPtr, funcType FuncPtr) : _class(ClassPtr), _funcPtr(FuncPtr)
			{
			}

#if SHARED_PTR_FUNCTIONS
			Func(Shared_Ptr<Class>&& ClassPtr, funcType FuncPtr) : _class(ClassPtr.Get()), _funcPtr(FuncPtr), _shared_class_ptr(std::move(ClassPtr))
			{
			}
#endif //SHARED_PTR_FUNCTIONS

			auto operator()(ArgT... Args) -> RT
			{
				return (_class->*_funcPtr)(std::forward<ArgT>(Args)...);
			}

			Func() = delete;
		};

		/*
		A function wrapper that can deal with static, non-static, and member functions.
		@param RT Return type of the function.
		@param Class [Class type that contains the function].
		@param ArgT Arguments that should be passed to the function.
		*/
		template <typename RT, typename Class, typename... ArgT>
		class Func<RT, Class, CONST, ArgT...>
		{
		public:
			//typedef RT(Class::* funcType)(ArgT...) const;
			using funcType = RT(Class::*)(ArgT...) const;

		private:
			Class* _class;
			funcType _funcPtr;

#if SHARED_PTR_FUNCTIONS
			Shared_Ptr<Class> _shared_class_ptr;
#endif //SHARED_PTR_FUNCTIONS

		public:
			Func(Class* ClassPtr, funcType FuncPtr) : _class(ClassPtr), _funcPtr(FuncPtr)
			{
			}

#if SHARED_PTR_FUNCTIONS
			Func(Shared_Ptr<Class>&& ClassPtr, funcType FuncPtr) : _class(ClassPtr.Get()), _funcPtr(FuncPtr), _shared_class_ptr(std::move(ClassPtr))
			{
			}
#endif //SHARED_PTR_FUNCTIONS

			auto operator()(ArgT... Args) const -> RT
			{
				return (_class->*_funcPtr)(std::forward<ArgT>(Args)...);
			}

			Func() = delete;
		};

		/*
		A function wrapper that can deal with static, non-static, and member functions.
		@param RT Return type of the function.
		@param Class [Class type that contains the function].
		@param ArgT Arguments that should be passed to the function.
		*/
		template <typename RT, typename Class, typename... ArgT>
		class Func<RT, Class, VOLATILE, ArgT...>
		{
		public:
			//typedef RT(Class::* funcType)(ArgT...) volatile;
			using funcType = RT(Class::*)(ArgT...) volatile;

		private:
			Class* _class;
			funcType _funcPtr;

#if SHARED_PTR_FUNCTIONS
			Shared_Ptr<Class> _shared_class_ptr;
#endif //SHARED_PTR_FUNCTIONS

		public:
			Func(Class* ClassPtr, funcType FuncPtr) : _class(ClassPtr), _funcPtr(FuncPtr)
			{
			}

#if SHARED_PTR_FUNCTIONS
			Func(Shared_Ptr<Class>&& ClassPtr, funcType FuncPtr) : _class(ClassPtr.Get()), _funcPtr(FuncPtr), _shared_class_ptr(std::move(ClassPtr))
			{
			}
#endif //SHARED_PTR_FUNCTIONS

			auto operator()(ArgT... Args) volatile -> RT
			{
				return (_class->*_funcPtr)(std::forward<ArgT>(Args)...);
			}

			Func() = delete;
		};

#pragma endregion RT, CLASS, PQ, ARGT

#pragma region RT, CLASS, PQ

		/*
		A function wrapper that can deal with static, non-static, and member functions.
		@param RT [Return type of the function].
		@param Class [Class type that contains the function].
		@param PQ [Post-qualifier for member function].
		*/
		template <typename RT, typename Class, Post_Qualifiers PQ>
		class Func<RT, Class, PQ>
		{
		public:
			//typedef RT(Class::* funcType)(void);
			using funcType = RT(Class::*)();

		private:
			Class* _class;
			funcType _funcPtr;

#if SHARED_PTR_FUNCTIONS
			Shared_Ptr<Class> _shared_class_ptr;
#endif // SHARED_PTR_FUNCTIONS


		public:
			Func(Class* ClassPtr, funcType FuncPtr) : _class(ClassPtr), _funcPtr(FuncPtr)
			{
			}

#if SHARED_PTR_FUNCTIONS
			Func(Shared_Ptr<Class>&& ClassPtr, funcType FuncPtr) : _class(ClassPtr.Get()), _funcPtr(FuncPtr), _shared_class_ptr(std::move(ClassPtr))
			{
			}
#endif // SHARED_PTR_FUNCTIONS

			auto operator()() -> RT
			{
				return (_class->*_funcPtr)();
			}

			Func() = delete;
		};

		/*
		A function wrapper that can deal with static, non-static, and member functions.
		@param RT [Return type of the function].
		@param Class [Class type that contains the function].
		*/
		template <typename RT, typename Class>
		class Func<RT, Class, CONST>
		{
		public:
			//typedef RT(Class::* funcType)(void) const;
			using funcType =  RT(Class::*)() const;

		private:
			Class* _class;
			funcType _funcPtr;

#if SHARED_PTR_FUNCTIONS
			Shared_Ptr<Class> _shared_class_ptr;
#endif // SHARED_PTR_FUNCTIONS


		public:
			Func(Class* ClassPtr, funcType FuncPtr) : _class(ClassPtr), _funcPtr(FuncPtr)
			{
			}

#if SHARED_PTR_FUNCTIONS
			Func(Shared_Ptr<Class>&& ClassPtr, funcType FuncPtr) : _class(ClassPtr.Get()), _funcPtr(FuncPtr), _shared_class_ptr(std::move(ClassPtr))
			{
			}
#endif // SHARED_PTR_FUNCTIONS

			auto operator()() const -> RT
			{
				return (_class->*_funcPtr)();
			}

			Func() = delete;
		};

		/*
		A function wrapper that can deal with static, non-static, and member functions.
		@param RT [Return type of the function].
		@param Class [Class type that contains the function].
		*/
		template <typename RT, typename Class>
		class Func<RT, Class, VOLATILE>
		{
		public:
			//typedef RT(Class::* funcType)(void) volatile;
			using funcType = RT(Class::*)() volatile;

		private:
			Class* _class;
			funcType _funcPtr;

#if SHARED_PTR_FUNCTIONS
			Shared_Ptr<Class> _shared_class_ptr;
#endif // SHARED_PTR_FUNCTIONS


		public:
			Func(Class* ClassPtr, funcType FuncPtr) : _class(ClassPtr), _funcPtr(FuncPtr)
			{
			}

#if SHARED_PTR_FUNCTIONS
			Func(Shared_Ptr<Class>&& ClassPtr, funcType FuncPtr) : _class(ClassPtr.Get()), _funcPtr(FuncPtr), _shared_class_ptr(std::move(ClassPtr))
			{
			}
#endif // SHARED_PTR_FUNCTIONS

			auto operator()() volatile -> RT
			{
				return (_class->*_funcPtr)();
			}

			Func() = delete;
		};

#pragma endregion RT, CLASS, PQ

		/*
		A function wrapper that can deal with static, non-static, and member functions.
		@param RT Return type of the function.
		@param ArgT Arguments that should be passed to the function.
		*/
		template <typename RT, typename... ArgT>
		class Func<RT, void, NONE, ArgT...>
		{
		public:
			//typedef RT(*funcType)(ArgT...);
			using funcType = RT(*)(ArgT...);

		private:
			funcType _funcPtr;

		public:
			Func(funcType FuncPtr) : _funcPtr(FuncPtr)
			{
			}

			auto operator()(ArgT... Args) -> RT
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
			//typedef RT(*funcType)(void);
			using funcType = RT(*)();

		private:
			funcType _funcPtr;

		public:
			Func(funcType FuncPtr) : _funcPtr(FuncPtr)
			{
			}

			auto operator()() -> RT
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
		static auto WrapFunction(typename Function::Func<void, void>::funcType FunctionPointer)
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
		static auto WrapFunction(typename Function::Func<RT, void, NONE, ArgT...>::funcType FunctionPointer)
		{
			return Function::Func<RT, void, NONE, ArgT...>(FunctionPointer);
		}

		/*
		* Wraps member function in a Func class.
		* @param RT [Return Type of the Function].
		* @param Class [Containing Class Type].
		* @param PQ [Post-qualifier for member function].
		* @param ArgT [Type of the Arguments Passed to the Function].
		* @param ClassPointer [Pointer to instance of Class].
		* @param FunctionPointer [Target Function]
		*/
		template<typename RT, typename Class, Post_Qualifiers PQ, typename... ArgT>
		static auto WrapFunction(Class* ClassPointer, typename Function::Func<RT, Class, PQ, ArgT...>::funcType FunctionPointer)
		{
			return Function::Func<RT, Class, PQ, ArgT...>(ClassPointer, FunctionPointer);
		}

#if SHARED_PTR_FUNCTIONS

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
#endif //SHARED_PTR_FUNCTIONS
#pragma endregion Func
	};
} //namespace ACBYTES
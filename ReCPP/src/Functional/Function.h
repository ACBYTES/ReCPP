#pragma once

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
		enum class Post_Qualifiers
		{
			//No post_qualifiers
			NONE,
			CONST,
			VOLATILE,
			//const volatile, LOL
			CONVOL
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
		template <typename RT, typename Class, Post_Qualifiers PQ = Post_Qualifiers::NONE, typename... ArgT>
		class Func
		{
		public:
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
			Func(Shared_Ptr<Class>&& ClassPtr, funcType FuncPtr) : _class(ClassPtr.Get()), _funcPtr(FuncPtr), _shared_class_ptr(ACBYTES::Move(ClassPtr))
			{
			}
#endif //SHARED_PTR_FUNCTIONS

			RT operator()(ArgT... Args)
			{
				return (_class->*_funcPtr)(Forward<ArgT>(Args)...);
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
		class Func<RT, Class, Post_Qualifiers::CONST, ArgT...>
		{
		public:
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
			Func(Shared_Ptr<Class>&& ClassPtr, funcType FuncPtr) : _class(ClassPtr.Get()), _funcPtr(FuncPtr), _shared_class_ptr(Move(ClassPtr))
			{
			}
#endif //SHARED_PTR_FUNCTIONS

			RT operator()(ArgT... Args) const
			{
				return (_class->*_funcPtr)(Forward<ArgT>(Args)...);
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
		class Func<RT, Class, Post_Qualifiers::VOLATILE, ArgT...>
		{
		public:
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
			Func(Shared_Ptr<Class>&& ClassPtr, funcType FuncPtr) : _class(ClassPtr.Get()), _funcPtr(FuncPtr), _shared_class_ptr(Move(ClassPtr))
			{
			}
#endif //SHARED_PTR_FUNCTIONS

			RT operator()(ArgT... Args) volatile
			{
				return (_class->*_funcPtr)(Forward<ArgT>(Args)...);
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
		class Func<RT, Class, Post_Qualifiers::CONVOL, ArgT...>
		{
		public:
			using funcType = RT(Class::*)(ArgT...) const volatile;

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
			Func(Shared_Ptr<Class>&& ClassPtr, funcType FuncPtr) : _class(ClassPtr.Get()), _funcPtr(FuncPtr), _shared_class_ptr(Move(ClassPtr))
			{
			}
#endif //SHARED_PTR_FUNCTIONS

			RT operator()(ArgT... Args) volatile
			{
				return (_class->*_funcPtr)(Forward<ArgT>(Args)...);
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
			Func(Shared_Ptr<Class>&& ClassPtr, funcType FuncPtr) : _class(ClassPtr.Get()), _funcPtr(FuncPtr), _shared_class_ptr(Move(ClassPtr))
			{
			}
#endif // SHARED_PTR_FUNCTIONS

			RT operator()()
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
		class Func<RT, Class, Post_Qualifiers::CONST>
		{
		public:
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
			Func(Shared_Ptr<Class>&& ClassPtr, funcType FuncPtr) : _class(ClassPtr.Get()), _funcPtr(FuncPtr), _shared_class_ptr(Move(ClassPtr))
			{
			}
#endif // SHARED_PTR_FUNCTIONS

			RT operator()() const
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
		class Func<RT, Class, Post_Qualifiers::VOLATILE>
		{
		public:
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
			Func(Shared_Ptr<Class>&& ClassPtr, funcType FuncPtr) : _class(ClassPtr.Get()), _funcPtr(FuncPtr), _shared_class_ptr(Move(ClassPtr))
			{
			}
#endif // SHARED_PTR_FUNCTIONS

			RT operator()() volatile
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
		class Func<RT, Class, Post_Qualifiers::CONVOL>
		{
		public:
			using funcType = RT(Class::*)() const volatile;

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
			Func(Shared_Ptr<Class>&& ClassPtr, funcType FuncPtr) : _class(ClassPtr.Get()), _funcPtr(FuncPtr), _shared_class_ptr(Move(ClassPtr))
			{
			}
#endif // SHARED_PTR_FUNCTIONS

			RT operator()() volatile
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
		class Func<RT, void, Post_Qualifiers::NONE, ArgT...>
		{
		public:
			using funcType = RT(*)(ArgT...);

		private:
			funcType _funcPtr;

		public:
			Func(funcType FuncPtr) : _funcPtr(FuncPtr)
			{
			}

			RT operator()(ArgT... Args)
			{
				return _funcPtr(Forward<ArgT>(Args)...);
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
			using funcType = RT(*)();

		private:
			funcType _funcPtr;

		public:
			Func(funcType FuncPtr) : _funcPtr(FuncPtr)
			{
			}

			RT operator()()
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
		static auto WrapFunction(typename Function::Func<RT, void, Post_Qualifiers::NONE, ArgT...>::funcType FunctionPointer)
		{
			return Function::Func<RT, void, Post_Qualifiers::NONE, ArgT...>(FunctionPointer);
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

		/*
		* Wraps member function in a Func class.
		* @param RT [Return Type of the Function].
		* @param Class [Containing Class Type].
		* @param ArgT [Type of the Arguments Passed to the Function].
		* @param ClassPointer [Pointer to instance of Class].
		* @param FunctionPointer [Target Function]
		*/
		template<typename RT, typename Class, typename... ArgT>
		static auto WrapFunction(Class* ClassPointer, typename Function::Func<RT, Class, Post_Qualifiers::NONE, ArgT...>::funcType FunctionPointer)
		{
			return Function::Func<RT, Class, Post_Qualifiers::NONE, ArgT...>(ClassPointer, FunctionPointer);
		}

#if SHARED_PTR_FUNCTIONS

		/*
		* Wraps member function in a Func class keeping a shared pointer copy, preventing the class instance from getting deleted.
		* @param RT [Return Type of the Function].
		* @param Class [Containing Class Type].
		* @param ArgT [Type of the Arguments Passed to the Function].
		* @param ClassPointer [Shared pointer to an instance of Class].
		* @param FunctionPointer [Target Function]
		*/
		template<typename RT, typename Class, typename... ArgT>
		static auto WrapFunction(Shared_Ptr<Class> ClassPointer, typename Function::Func<RT, Class, Post_Qualifiers::NONE, ArgT...>::funcType FunctionPointer)
		{
			return Function::Func<RT, Class, Post_Qualifiers::NONE, ArgT...>(Move(ClassPointer), FunctionPointer);
		}

		/*
		* Wraps member function in a Func class keeping a shared pointer copy, preventing the class instance from getting deleted.
		* @param RT [Return Type of the Function].
		* @param Class [Containing Class Type].
		* @param PQ [Target member function's post-qualifiers]
		* @param ArgT [Type of the Arguments Passed to the Function].
		* @param ClassPointer [Shared pointer to an instance of Class].
		* @param FunctionPointer [Target Function]
		*/
		template<typename RT, typename Class, Post_Qualifiers PQ, typename... ArgT>
		static auto WrapFunction(Shared_Ptr<Class> ClassPointer, typename Function::Func<RT, Class, PQ, ArgT...>::funcType FunctionPointer)
		{
			return Function::Func<RT, Class, PQ, ArgT...>(Move(ClassPointer), FunctionPointer);
		}
#endif //SHARED_PTR_FUNCTIONS
#pragma endregion Func
	};
} //namespace ACBYTES
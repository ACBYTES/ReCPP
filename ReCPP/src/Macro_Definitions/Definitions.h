/*
* Deletes all of the target class'/struct's default constructors.
*/
#define NO_DEFAULT_CONSTRUCTORS(ClassName) ClassName() = delete; ClassName(const ClassName&) = delete; ClassName(ClassName&&) = delete
/*
* Deletes all of the target class'/struct's default constructors.
*/
#define NO_DEFAULT_CONSTRUCTORS(TypeName) TypeName() = delete; TypeName(const TypeName&) = delete; TypeName(TypeName&&) = delete
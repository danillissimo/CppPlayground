#pragma once

//Ignore all params
#define ignore(...)
//Insert betwen a function-like macro and its arguments so that other macros can be parsed before the obstructed
#define obstruct(...)
//Turn a ... into a single argument
#define bake(...) __VA_ARGS__

// Unused atm //
/*************************************************************************************************/

#pragma region Unused atm

// Force preprocessor to parse an expression again
// #define parse(...) __VA_ARGS__

//
//#define defer(op) op obstruct()

// Same as ignore(...), but may improve readability sometimes: better expresses the idea there must be completly nothing
// #define empty(...)

//
// #define concat(a, b) a##b

#pragma endregion ~ Unused atm

/*************************************************************************************************/

#define begin_args() (
#define end_args() )

// Unused atm
// #define next_arg() ,
// #define reference() &
// #define end_line() ;

#define make_args_begining_do (
#define make_args_begining_not
#define make_args_begining(do_or_not) make_args_begining_##do_or_not

#define make_next_arg_do ,
#define make_next_arg_not
#define make_next_arg(do_or_not) make_next_arg##do_or_not

#define make_ref_suffix() _ref
#define make_ptr_suffix() _ptr
#define make_cpp_suffix() _cpp

#define do_wrap_with_figure_braces_(expression) {expression}
#define do_wrap_with_figure_braces_not(expression) expression
#define wrap_with_figure_braces(expression, ...) do_wrap_with_figure_braces_##__VA_ARGS__ (expression)

#define make_getter_(type, name, ...) type Get##name () const { return name; }
#define make_getter_custom(type, name, body, ...) do_make_getter_custom##__VA_ARGS__ (type, name, body)
#define do_make_getter_custom(type, name, body) type Get##name () const body
#define do_make_getter_custom_ref(type, name, ...) const type & Get##name () const { return name ; }
#define do_make_getter_custom_ptr(type, name, ...) const type * Get##name () const { return & name ; }
#define do_make_getter_custom_cpp(type, name, ...) type Get##name () const ;
#define make_argument_get(type, name, arg, ...) make_getter_##arg obstruct() (type, name, __VA_ARGS__)

#define make_setter_(type, name, ...) void Set##name (type value) { name = value; }
#define make_setter_custom(type, name, body, ...) do_make_setter_custom##__VA_ARGS__ (type, name, body)
#define do_make_setter_custom(type, name, body) void Set##name (type value) body
#define do_make_setter_custom_ref(type, name, ...) void Set##name (const type & value) { name = value; }
#define do_make_setter_custom_ptr(type, name, ...) void Set##name (const type * value) { name = *value; }
#define do_make_setter_custom_cpp(type, name, ...) void Set##name (const type& value) ;
#define make_argument_set(type, name, arg, ...) make_setter_##arg obstruct() (type, name, __VA_ARGS__)

#define make_argument_public(type, name, ...) public:
#define make_argument_protected(type, name, ...) protected:
#define make_argument_private(type, name, ...) private:

#define make_argument_(...)
//TODO Remove suppressors after fixing the main "field" macro
//#define make_argument_custom(...)
//#define make_argument_bake(...)
//#define make_argument__ptr(...)
//#define ignored_body(...)
//#define make_argument__ref(...)
//#define make_argument__cpp(...)

#define do_make_argument(type, name, arg, ...) make_argument_##arg (type, name, __VA_ARGS__)

// Purely cosmetic and very satisfying
#define get get,
#define set set,

// Turn basic argument builder into custom and pass args to it
// Bake user code so it can contain ","
#define as(...) custom, wrap_with_figure_braces obstruct()(bake obstruct obstruct obstruct obstruct()()()() (__VA_ARGS__))
// Hack the preceding "as" above from within its __VA_ARGS__
// Escape from "bake()" so that all new statements can be parsed later - add a ")", eliminate its floating pair by making it a part of "ignore()"
// wrap_with_figure_braces(code, ) turns into wrap_with_figure_braces( , not), effectively producing nothing
// "ignored_body" becomes ignored argument builder's __VA_ARGS__
// make_***_suffix is appended to argument builder's name later during make_%whatever%_custom phase
//
// Yes, this was a veeeeery tricky part, easier to believe and watch it work
#define ref ( end_args obstruct()() ignored_body, not ignore begin_args obstruct()() ) , make_ref_suffix obstruct obstruct obstruct ()()() ()
#define ptr ( end_args obstruct()() ignored_body, not ignore begin_args obstruct()() ) , make_ptr_suffix obstruct obstruct obstruct ()()() ()
#define cpp ( end_args obstruct()() ignored_body, not ignore begin_args obstruct()() ) , make_cpp_suffix obstruct obstruct obstruct ()()() ()
//Works as well
//#define cpp ( end_args obstruct()() ;, not ignore begin_args obstruct()() )

#define make_argument(type, name, expression) do_make_argument obstruct()(type,name,expression)

// TODO Something must be done for cases when there are lees then 4 args - it IS supported, but compilers are aching for all of them
// But I was already too lazy to deal with it
#define field(type, name, arg1, arg2, arg3, arg4) \
	make_argument(type, name, arg1)\
	make_argument(type, name, arg2)\
	make_argument(type, name, arg3)\
	make_argument(type, name, arg4)\
	type name

/* Wellp, tried recursion - failed, but may be rebuild later */
// #define take2args(arg1, arg2, ...) 2ARGS_BEGIN arg1, arg2 2ARGS_END
// #define skip2args(arg1, arg2, ...) __VA_ARGS__
// #define take3args(arg1, arg2, arg3, ...) arg1, arg2, arg3
// #define skip3args(arg1, arg2, arg3, ...) __VA_ARGS__
// #define take4args(arg1, arg2, arg3, arg4, ...) arg1, arg2, arg3, arg4
// #define skip4args(arg1, arg2, arg3, arg4, ...) __VA_ARGS__
// #define take5args(arg1, arg2, arg3, arg4, arg5, ...) 5ARGS_BEGIN arg1, arg2, arg3, arg4, arg5 5ARGS_END
// #define skip5args(arg1, arg2, arg3, arg4, arg5, ...) __VA_ARGS__
//
// #define  make1args(type, name, argument)        make_argument(type, name, argument) type name
// #define  make2args(type, name, argument, ...)   make_argument(type, name, argument)       make1args  (type, name, take4args(__VA_ARGS__), skip4args(__VA_ARGS__))
// #define  make3args(type, name, argument, ...)   make_argument(type, name, argument)       make2args  (type, name, take4args(__VA_ARGS__), skip4args(__VA_ARGS__))
// #define  make4args(type, name, argument, ...)   make_argument(type, name, argument)       make3args  (type, name, take4args(__VA_ARGS__), skip4args(__VA_ARGS__))
// #define  make5args(type, name, argument, ...)   make_argument(type, name, argument)       make4args  (type, name, take4args(__VA_ARGS__), skip4args(__VA_ARGS__))
// #define  make6args(type, name, argument, ...)   make_argument(type, name, argument)       make5args  (type, name, take4args(__VA_ARGS__), skip4args(__VA_ARGS__))
// #define  make7args(type, name, argument, ...)   make_argument(type, name, argument)       make6args  (type, name, take4args(__VA_ARGS__), skip4args(__VA_ARGS__))
// #define  make8args(type, name, argument, ...)   make_argument(type, name, argument)       make7args  (type, name, take4args(__VA_ARGS__), skip4args(__VA_ARGS__))
// #define  make9args(type, name, argument, ...)   make_argument(type, name, argument)       make8args  (type, name, take4args(__VA_ARGS__), skip4args(__VA_ARGS__))
// #define make10args(type, name, argument, ...)   make_argument(type, name, argument)       make9args  (type, name, take4args(__VA_ARGS__), skip4args(__VA_ARGS__))
// #define make11args(type, name, argument, ...)   make_argument(type, name, argument)       make10args (type, name, take4args(__VA_ARGS__), skip4args(__VA_ARGS__))
// #define make12args(type, name, argument, ...)   make_argument(type, name, argument)       make11args (type, name, take4args(__VA_ARGS__), skip4args(__VA_ARGS__))
// #define make13args(type, name, argument, ...)   make_argument(type, name, argument)       make12args (type, name, take4args(__VA_ARGS__), skip4args(__VA_ARGS__))
// #define make14args(type, name, argument, ...)   make_argument(type, name, argument)       make13args (type, name, take4args(__VA_ARGS__), skip4args(__VA_ARGS__))
// #define make15args(type, name, argument, ...)   make_argument(type, name, argument)       make14args (type, name, take4args(__VA_ARGS__), skip4args(__VA_ARGS__))
// #define make16args(type, name, argument, ...)   make_argument(type, name, argument)       make15args (type, name, take4args(__VA_ARGS__), skip4args(__VA_ARGS__))
// #define make17args(type, name, argument, ...)   make_argument(type, name, argument)       make16args (type, name, take4args(__VA_ARGS__), skip4args(__VA_ARGS__))
//#define field(type, name, first_argument, ...) make_argument(type, name, first_argument)  make17args (type, name, take3args(__VA_ARGS__), skip4args(__VA_ARGS__))
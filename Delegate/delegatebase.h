#include <type_traits>

/*
 * Invocation by reference should be faster, but takes 3 pointers.
 * Invocation by selector should be slower, and requires 2 pointers and a char, but is noticeable
 * only with #pragma pack(1) on x32, works much better on x64.
 */

//=================================================================================================

//#define DELEGATE_DEBUG

#ifdef DELEGATE_DEBUG
#    pragma message("WARNING, DELEGATES ARE IN DEBUG MODE")

#    undef DELEGATE_NAME
#    undef DELEGATE_INVOKE_BY_REFERENCE
#    undef DELEGATE_SUPPORT_STATIC
#    undef DELEGATE_SUPPORT_NONSTATIC

#    define DELEGATE_NAME                AbstractDelegate
#    define DELEGATE_INVOKE_BY_REFERENCE 0
#    define DELEGATE_SUPPORT_STATIC      1
#    define DELEGATE_SUPPORT_NONSTATIC   1

#endif

#if DELEGATE_SUPPORT_STATIC == 0 && DELEGATE_SUPPORT_NONSTATIC == 0
#    error DELEGATE_SUPPORT_NONSTATIC either DELEGATE_SUPPORT_STATIC must be 1 when declaring a new delegate type
#endif

//=================================================================================================


//#ifndef forceinline
//#    ifdef _MSC_VER
//#        define forceinline __forceinline
//#    else
//#        define forceinline __attribute__((always_inline))
//#    endif
//#endif

#ifndef DELEGATE_CONVENSIONS_DEFINED
#    define DELEGATE_CONVENSIONS_DEFINED
#    ifdef _MSC_VER
#        define delegate_stdcall __stdcall
#    else
#        define delegate_stdcall __attribute__((stdcall))
#    endif

#    ifdef _MSC_VER
#        define delegate_fastcall __fastcall
#    else
#        define delegate_fastcall __attribute__((fastcall))
#    endif

#    ifdef _MSC_VER
#        define delegate_thiscall __thiscall
#    else
#        define delegate_thiscall __attribute__((thiscall))
#    endif

#    ifdef _MSC_VER
#        define delegate_vectorcall __vectorcall
#    else
#        define delegate_vectorcall __attribute__((vectorcall))
#    endif

#    ifdef _MSC_VER
#        define delegate_cdecl __cdecl
#    else
#        define delegate_cdecl __attribute__((cdecl))
#    endif

#    ifdef _MANAGED
#        define delegate_clrcall __clrcall
#    endif

//__regcall
//__pascal
//__fortran
//__syscall

#endif

//=================================================================================================

#if _WIN64 || __x86_64__ || __ppc64__
// cdecl is important here, as the rest of conventions are turned into this one, leading to correct
// preprocessor definitions, but compiler looking exactly for cdecl
//
// TODO: x64 delegate certainly deserves its own specialization. Though it can be a good idea to
// allow any delegate specialize
#    ifdef _MANAGED
#        define DELEGATE_IMPLEMENT_ALL                                                             \
            IMPLEMENT(delegate_cdecl)                                                              \
            IMPLEMENT(delegate_vectorcall)                                                         \
            IMPLEMENT(delegate_clrcall)
#    else
#        define DELEGATE_IMPLEMENT_ALL                                                             \
            IMPLEMENT(delegate_cdecl)                                                              \
            IMPLEMENT(delegate_vectorcall)
#    endif
#else
#    ifdef _MANAGED
#        define DELEGATE_IMPLEMENT_ALL                                                             \
            IMPLEMENT(delegate_stdcall)                                                            \
            IMPLEMENT(delegate_cdecl)                                                              \
            IMPLEMENT(delegate_fastcall)                                                           \
            IMPLEMENT(delegate_thiscall)                                                           \
            IMPLEMENT(delegate_vectorcall)                                                         \
            IMPLEMENT(delegate_clrcall)
#    else
#        define DELEGATE_IMPLEMENT_ALL                                                             \
            IMPLEMENT(delegate_stdcall)                                                            \
            IMPLEMENT(delegate_cdecl)                                                              \
            IMPLEMENT(delegate_fastcall)                                                           \
            IMPLEMENT(delegate_thiscall)                                                           \
            IMPLEMENT(delegate_vectorcall)
#    endif
#endif

#define delegate_str_concat(a, b) a##b

#define delegate_make_caller_name(conventionName) delegate_str_concat(cc_, conventionName)
#define delegate_make_static_method_name(conventionName)                                           \
    delegate_str_concat(static_, conventionName)
#define delegate_make_nonstatic_method_name(conventionName)                                        \
    delegate_str_concat(member_, conventionName)

//=================================================================================================

template<typename ReturnType, typename... FunctionArguments>
struct DELEGATE_NAME
{
  protected:
    /*
     * One can't invoke methods for a void*, thus some dummy class required.
     */
    class Dummy
    {};

    /**********************************************************************************************
     * Implementation of convention wrapper and how it's processed
     *********************************************************************************************/

#if DELEGATE_INVOKE_BY_REFERENCE == 1
    /*
     * Procedure based
     */

#    define UPDATE_SELECTOR(value) caller = &DELEGATE_NAME<ReturnType, FunctionArguments...>::value

#    if DELEGATE_SUPPORT_STATIC == 1 && DELEGATE_SUPPORT_NONSTATIC == 1

#        define IMPLEMENT(conventionName)                                                          \
            ReturnType delegate_make_caller_name(conventionName)(FunctionArguments... args) const  \
            {                                                                                      \
                if (entity != nullptr)                                                             \
                    return (*entity.*(method.delegate_make_nonstatic_method_name(                  \
                                         conventionName)))(args...);                               \
                return (*(method.delegate_make_static_method_name(conventionName)))(args...);      \
            }

#    elif DELEGATE_SUPPORT_NONSTATIC == 1

#        define IMPLEMENT(conventionName)                                                          \
            ReturnType delegate_make_caller_name(conventionName)(FunctionArguments... args) const  \
            {                                                                                      \
                return (*entity.*                                                                  \
                        (method.delegate_make_nonstatic_method_name(conventionName)))(args...);    \
            }

#    elif DELEGATE_SUPPORT_STATIC == 1

#        define IMPLEMENT(conventionName)                                                          \
            ReturnType delegate_make_caller_name(conventionName)(FunctionArguments... args) const  \
            {                                                                                      \
                return (*(method.delegate_make_static_method_name(conventionName)))(args...);      \
            }
#    endif

    DELEGATE_IMPLEMENT_ALL;
#else
    /*
     * Enum based
     */
#    define IMPLEMENT(conventionName) delegate_make_caller_name(conventionName),
    enum CallingConvetion : char
    {
        DELEGATE_IMPLEMENT_ALL
    };

#    define UPDATE_SELECTOR(value) convention = value
#endif
#undef IMPLEMENT

    /**********************************************************************************************
     * Fields:
     * - Object pointer
     * - Union for storing method pointers
     * - Convention wrapper reference
     *********************************************************************************************/


    // Object pointer::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#if DELEGATE_SUPPORT_NONSTATIC == 1
    Dummy * entity;
#endif


    // Union for storing method pointers:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#define IMPLEMENT(conventionName)                                                                  \
    ReturnType (conventionName Dummy::*delegate_make_nonstatic_method_name(conventionName))(       \
        FunctionArguments...);                                                                     \
    ReturnType(conventionName * delegate_make_static_method_name(conventionName))(                 \
        FunctionArguments...);

    union
    {
        DELEGATE_IMPLEMENT_ALL
    } method;
#undef IMPLEMENT


    // Convention wrapper reference::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#if DELEGATE_INVOKE_BY_REFERENCE == 1
    ReturnType (DELEGATE_NAME<ReturnType, FunctionArguments...>::*caller)(
        FunctionArguments...) const;
#else
    CallingConvetion convention;
#endif

    /**********************************************************************************************
     * Public interface
     * - Getters
     * - Assign
     * - Invoke
     *********************************************************************************************/

  public:
    // Getters:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#if DELEGATE_SUPPORT_NONSTATIC == 1
    void * getTarget() const { return entity; }
#endif

    // Assign::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#if DELEGATE_SUPPORT_STATIC == 1 && DELEGATE_SUPPORT_NONSTATIC == 1

#    define IMPLEMENT(conventionName)                                                              \
        template<typename T>                                                                       \
        void assign(const T & object,                                                              \
                    ReturnType (conventionName T::*function)(FunctionArguments...))                \
        {                                                                                          \
            entity = ( Dummy * )&object;                                                           \
            method.delegate_make_nonstatic_method_name(conventionName) =                           \
                (ReturnType(conventionName Dummy::*)(FunctionArguments...))(function);             \
            UPDATE_SELECTOR(delegate_make_caller_name(conventionName));                            \
        }                                                                                          \
                                                                                                   \
        void assign(ReturnType(conventionName * function)(FunctionArguments...))                   \
        {                                                                                          \
            entity                                                  = nullptr;                     \
            method.delegate_make_static_method_name(conventionName) = function;                    \
            UPDATE_SELECTOR(delegate_make_caller_name(conventionName));                            \
        }

#elif DELEGATE_SUPPORT_NONSTATIC == 1

#    define IMPLEMENT(conventionName)                                                              \
        template<typename T>                                                                       \
        void assign(const T & object,                                                              \
                    ReturnType (conventionName T::*function)(FunctionArguments...))                \
        {                                                                                          \
            entity = ( Dummy * )&object;                                                           \
            method.delegate_make_nonstatic_method_name(conventionName) =                           \
                (ReturnType(conventionName Dummy::*)(FunctionArguments...))(function);             \
            UPDATE_SELECTOR(delegate_make_caller_name(conventionName));                            \
        }

#elif DELEGATE_SUPPORT_STATIC == 1

    // Does nothing about 'entity' ptr as it doesn't exist
#    define IMPLEMENT(conventionName)                                                              \
        void assign(ReturnType(conventionName * function)(FunctionArguments...))                   \
        {                                                                                          \
            method.delegate_make_static_method_name(conventionName) = function;                    \
            UPDATE_SELECTOR(delegate_make_caller_name(conventionName));                            \
        }

#endif

    DELEGATE_IMPLEMENT_ALL
#undef IMPLEMENT
#undef UPDATE_SELECTOR

#define assign_delegate(object, method)                                                            \
    assign(object, &std::remove_reference<std::remove_cv<decltype(object)>::type>::type::method)

    // Invoke::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#if DELEGATE_INVOKE_BY_REFERENCE == 1

    ReturnType invoke(FunctionArguments... args) const { return (*this.*caller)(args...); }

#else

  private:
    ReturnType invokeNonstatic(FunctionArguments... args) const
    {
        switch (convention)
        {
#    define IMPLEMENT(conventionName)                                                              \
        case delegate_make_caller_name(conventionName):                                            \
            return (*entity.*(method.delegate_make_nonstatic_method_name(conventionName)))(args...);
            DELEGATE_IMPLEMENT_ALL;
#    undef IMPLEMENT
        }
    }

    ReturnType invokeStatic(FunctionArguments... args) const
    {
        switch (convention)
        {
#    define IMPLEMENT(conventionName)                                                              \
        case delegate_make_caller_name(conventionName):                                            \
            return (*(method.delegate_make_static_method_name(conventionName)))(args...);
            DELEGATE_IMPLEMENT_ALL;
#    undef IMPLEMENT
        }
    }

  public:

#    if DELEGATE_SUPPORT_STATIC == 1 && DELEGATE_SUPPORT_NONSTATIC == 1

    ReturnType invoke(FunctionArguments... args) const
    {
        if (entity != nullptr)
        {
            return invokeNonstatic(args...);
        }
        else
        {
            return invokeStatic(args...);
        }
    }

#    elif DELEGATE_SUPPORT_NONSTATIC == 1

    ReturnType invoke(FunctionArguments... args) const { return invokeNonstatic(args...); }

#    elif DELEGATE_SUPPORT_STATIC == 1

    ReturnType invoke(FunctionArguments... args) const { return invokeStatic(args...); }

#    endif

#endif

    /**********************************************************************************************
     * std::hash and std::equal_to
     *********************************************************************************************/

  public:
    // hash::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    struct hash
    {
      private:
        _NODISCARD size_t static getConventionDescriptor(
            const DELEGATE_NAME<ReturnType, FunctionArguments...> & value) noexcept
        {
#if DELEGATE_INVOKE_BY_REFERENCE == 1
            return (size_t)(*(( void ** )(&(value.method))));
#else
            return ( size_t )value.convention;
#endif
        }

        _NODISCARD
        size_t static getPtr(const DELEGATE_NAME<ReturnType, FunctionArguments...> & value) noexcept
        {
#if DELEGATE_SUPPORT_NONSTATIC
            return ( size_t )value.entity;
#else
            return 0;
#endif
        }

      public:
        _NODISCARD size_t
        operator()(const DELEGATE_NAME<ReturnType, FunctionArguments...> & value) const noexcept
        {
            return getPtr(value) + getConventionDescriptor(value);
        }
    };

    // equal_to::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    struct equal_to
    {
      private:
        _NODISCARD constexpr bool static checkConvention(
            const DELEGATE_NAME<ReturnType, FunctionArguments...> & d1,
            const DELEGATE_NAME<ReturnType, FunctionArguments...> & d2) noexcept
        {
#if DELEGATE_INVOKE_BY_REFERENCE == 1
            return d1.method.delegate_make_nonstatic_method_name(delegate_cdecl) ==
                   d2.method.delegate_make_nonstatic_method_name(delegate_cdecl);
#else
            return d1.convention == d2.convention;
#endif
        }

        _NODISCARD constexpr bool static checkObject(
            const DELEGATE_NAME<ReturnType, FunctionArguments...> & d1,
            const DELEGATE_NAME<ReturnType, FunctionArguments...> & d2) noexcept
        {
#if DELEGATE_SUPPORT_NONSTATIC == 1
            return d1.entity == d2.entity;
#else
            return true;
#endif
        }

      public:
        _NODISCARD constexpr bool
        operator()(const DELEGATE_NAME<ReturnType, FunctionArguments...> & d1,
                   const DELEGATE_NAME<ReturnType, FunctionArguments...> & d2) const noexcept
        {
            return checkConvention(d1, d2) && checkObject(d1, d2);
        }
    };
};

#undef DELEGATE_IMPLEMENT_ALL

#undef delegate_str_concat
#undef delegate_make_caller_name
#undef delegate_make_static_method_name
#undef delegate_make_nonstatic_method_name

#undef DELEGATE_NAME
#undef DELEGATE_INVOKE_BY_REFERENCE
#undef DELEGATE_SUPPORT_STATIC
#undef DELEGATE_SUPPORT_NONSTATIC
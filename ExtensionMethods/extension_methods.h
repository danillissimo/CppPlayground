#define DeclareExtensionMethod(ExtendedTypeName, ReturnType, MethodName, Param1Type, Param1Name, Param2Type, Param2Name)\
namespace ExtensionMethods\
{\
    struct MethodName##Args{\
        MethodName##Args(Param1Type Param1Name, Param2Type Param2Name)\
            : Param1Name(Param1Name)\
            , Param2Name(Param2Name)\
        {}\
        \
        Param1Type Param1Name;\
        Param2Type Param2Name;\
    };\
}\
\
ExtensionMethods::MethodName##Args MethodName(Param1Type Param1Name, Param2Type Param2Name)\
{\
    return ExtensionMethods::MethodName##Args(Param1Name, Param2Name);\
}\
\
static ReturnType exec##MethodName(ExtendedTypeName & This, Param1Type Param1Name, Param2Type Param2Name);\
\
static ReturnType operator->* (ExtendedTypeName & This, ExtensionMethods::MethodName##Args Args){\
    return exec##MethodName(This, Args.Param1Name, Args.Param2Name);\
}\
\
static ReturnType exec##MethodName(ExtendedTypeName & This, Param1Type Param1Name, Param2Type Param2Name)
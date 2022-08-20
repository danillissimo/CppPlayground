Deriving from underivable when patching the engine is not option. Suitable for editor and development builds. Can easily be modified for more generic C++ needs. 

Adds macros for calling `Super` methods via GetDefault<>()'s VTable on `this`:
- `FVTable__CallSuper(FunctionName, Params...)`
- `FVTable__CallSuperOverload(FunctionPointer, Params...)`
- `FVTable__CallSuperInterface(FunctionName, Params...)`
- `FVTable__CallSuperInterfaceOverload(FunctionPointer, Params...)`
- `SelectOverload(FunctionName, ParamTypes...)` to be used with `Overload`-versioned macros in place of `FunctionPointer` argument. Can probably be omitted in the future.

Use with caution as `this` turns into a `Super` during such calls
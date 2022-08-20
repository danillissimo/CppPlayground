#pragma once

#ifndef FORCEINLINE
#ifdef _MSC_VER
#define FORCEINLINE __forceinline
#else
#define FORCEINLINE __attribute__((always_inline))
#endif
#endif

#ifndef DEBUGBREAK
#ifdef _MSC_VER
//Check UE4 PLATFORM_BREAK for windows for __nop() explanation
#include <intrin.h>
#define DEBUGBREAK (__nop(), __debugbreak())
#else
#define DEBUGBREAK __asm__ volatile("int $0x03")
#endif
#endif

//=================================================================================================

#define OBSERVE_PER_INSTANCE 1

#if OBSERVE_PER_INSTANCE == 0

/**
* Turns a "var" into a "var()", allowing to place a breakpoint on it and observe all reads and
* writes, or configure breakpoints on per-instance basis from code depending on OBSERVE_PER_INSTANCE.
* It can't differentiate reads from writes though, just the fact of access to the field.
* Should only work in DEBUG builds, as compiler will strip out this boilerplate, though per-instance
* observations are more complex, and may not be compeltly removed from code when compiling, so
* production builds are recommended with OBSERVE_PER_INSTANCE set to 0.
* Raw field is still accessible trough __value, if you want a certain line of code not to
* cause breaks.
* Don't forget to implement static class fields with "implementObservableInClass" macro.
*
* @param type Type of the defined variable.
* @param name Name of the defiend variable.
* @param observeThisInstance Optional. Whether this instance should be observed from begining or
*                                      not. Only works with OBSERVE_PER_INSTANCE.
* @param observeWholeClass Optional. Overrides observeThisInstance. Defines whether all instances
*									 should be observed from begining or not. Only works with
*									 OBSERVE_PER_INSTANCE.
*/
#define observe(type, name, ...)\
struct __##name##Observer{ \
public:\
	type __value; \
	\
	FORCEINLINE const type& operator()() const { return __value; }\
	FORCEINLINE type& operator()() { return __value; }\
	FORCEINLINE void setObserved(bool value) const { }\
	FORCEINLINE void setClassObserved(bool value) const { }\
} name

#define implementObservableInClass(...)

#else

/*
* Static field is stored in a separate structure so it can work both in a method and in a class.
* setObserved is declared const for development purposes, so that one doesn't have to mess with const 
* context.
*/

/**
* Turns a "var" into a "var()", allowing to place a breakpoint on it and observe all reads and
* writes, or configure breakpoints on per-instance basis from code depending on OBSERVE_PER_INSTANCE.
* It can't differentiate reads from writes though, just the fact of access to the field.
* Should only work in DEBUG builds, as compiler will strip out this boilerplate, though per-instance
* observations are more complex, and may not be compeltly removed from code when compiling, so
* production builds are recommended with OBSERVE_PER_INSTANCE set to 0.
* Raw field is still accessible trough __value, if you want a certain line of code not to
* cause breaks.
* Don't forget to implement static class fields with "implementObservableInClass" macro.
*
* @param type Type of the defined variable.
* @param name Name of the defiend variable.
* @param observeThisInstance Optional. Whether this instance should be observed from begining or
*                                      not. Only works with OBSERVE_PER_INSTANCE.
* @param observeWholeClass Optional. Overrides observeThisInstance. Defines whether all instances
*									 should be observed from begining or not. Only works with
*									 OBSERVE_PER_INSTANCE.
*/
#define observe(type, name, ...)\
struct __##name##__ObserverStaticDataWrapper {\
public:\
	bool __areAll__##name##__ObserversObserved;\
};\
static __##name##__ObserverStaticDataWrapper __##name##__ObserverStaticDataWrapperInstance;\
struct __##name##__Observer {\
private:\
	mutable bool isObserved;\
	FORCEINLINE void init(bool inObserved = true, bool inClassObserved = false) {\
		isObserved = inObserved;\
		__##name##__ObserverStaticDataWrapperInstance.__areAll__##name##__ObserversObserved = inClassObserved;\
	}\
	FORCEINLINE void _break() const { if(isObserved || __##name##__ObserverStaticDataWrapperInstance.__areAll__##name##__ObserversObserved) { DEBUGBREAK; } }\
public:\
	type __value;\
	FORCEINLINE __##name##__Observer() { init(__VA_ARGS__); }\
	\
	FORCEINLINE const type& operator()() const { _break(); return __value; }\
	FORCEINLINE type& operator()() { _break(); return __value; }\
	\
	FORCEINLINE void setObserved(bool value) const { isObserved = value; }\
	FORCEINLINE void setClassObserved(bool value) const { __##name##__ObserverStaticDataWrapperInstance.__areAll__##name##__ObserversObserved = value; }\
} name

#define implementObservableInClass(className, variableName) className::__##variableName##__ObserverStaticDataWrapper className::__##variableName##__ObserverStaticDataWrapperInstance

#endif
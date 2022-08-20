# Description
An attempt to implement C#-like fields with getters and setters in C++. Not usable IRL as isn't debuggable (and will probably never be) and plastic enough (atm), and drains any coding assistants crazy, **but** was a big ~~pain~~ fun to implement (or understand), and I find the result quite impressive - it's really laconic and flexible, just like in C#, despite all the commas, required for the macro to work. Believe it or not - next lines are totally valid: (!)
```cpp
class Tester
{
public:
	field(int, A, get);
	field(int, B, set);
	field(int, C, get, set);
	field(int, CC, set, get);

	field(int, D, public, get, public, set);
	field(int, E, public, get, private, set);
	field(int, F, private, get, public, set);
	field(int, G, private, get, private, set);

public:
	field(int, H, get as(return H;), set as(H = value + 1;));
	field(int, I, get as cpp, set as cpp);
	field(int, J, get as ref, set as ref);
	field(int, K, get as ptr, set as ptr);

	// Some wildcard combinations here
	field(std::string, W1, private, set as ref, public, get as cpp);
	field(const char*, W2, get as(return "W2";));
	field(float, W3, set, get as ptr);
};
```
# Syntax
`field(type, name, arguments...)` where
- `Type` is the type of the field
- `Name` is the name of the field
- `Arguments...` is 0 to 4 arguments of supported types in no particular order which can be
- - Access modifiers `private` or `public` (without ':')
- - `set` or `get`, which generates respectively `void SetName(type value)` and `type GetName() const`, and can be followed by
- - * nothing, leading to generation of default getter/setter
- - * `as( custom_code; )` (no top-level figure braces required), where custom code does the stuff, and setters can also access `value` argument
- - * `as cpp`, telling that it's only a declaration, and implementation is elsewhere (probably in a *cpp* file)
- - * `as ref`, leading to generation of default getter/setter, but operating on references to provided type rather than the type itself
- - * `as ptr`, leading to generation of default getter/setter, but operating on pointers to provided type rather than the type itself

At the moment you'll probably want to always specify access modifiers, as `field` generates the underlying lvalue as well, and it will be affected by the very last access modifier passed to the macro.

*Please consult your memetic safety curator before opening the source code.*

*Only works under MSVC - other compilers may require massive modifications.*
An attempt to implement extension methods in C++. Rather successful, but requires examination of generated machine code before using IRL. Also requires some modification to support a  variadic number of arguments, or a set of predefined templates - sample only supports no less and no more then 2 arguments. Example:
```cpp
class Test{
public:
    int Val;
};

#include "extension_methods.h"

DeclareExtensionMethod(
    Test, 
    int, calc, int, m, int, d)
{
    return This.Val * m / d;
}

#include <iostream>

int main(){
    Test test;
    test.Val = 10;
    std::cout << test->*calc(3,2);
}
```
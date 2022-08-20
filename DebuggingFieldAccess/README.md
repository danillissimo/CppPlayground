Just an idea on debugging variable access as if we were some ancient programmers. Nothing serious, though requires a bit of skill to implement. Still has some limitations, but, again - nothing serious here. Sample:
```cpp
#include "observe.h"

class Test{
public:
  observe(int, observable);
};

implementObservableInClass(Test, observable);

int main()
{
    Test test;
    test.observable() += 123;
    
    observe(int, observable);
    observable() += 123;

    return 0;
}
```
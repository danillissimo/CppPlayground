#include <iostream>

#pragma pack(1)
#include "delegate.h"
#include "eventbus.h"

class TestA
{
public:
	void doTestA()
	{
		std::cout << "test a\n";
	}
};

class TestB
{
public:
	void doTestB()
	{
		std::cout << "test b\n";
	}
};

int main()
{
	EventBus<void> bus;
	TestA a;
	TestB b;
	Delegate<void> ad, bd;
	ad.assign_delegate(a, doTestA);
	bd.assign_delegate(b, doTestB);
	bus.subscribe(ad);
	bus.subscribe(bd);
	bus.fireEvent();
    return 0;
}
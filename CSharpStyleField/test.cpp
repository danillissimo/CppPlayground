#include <iostream>
#include "field.h"

// Yeah, absolutely sure could fix this, but was too lazy already
// GCC requires something similar to handle this, or may be a full fix, or even own implementation, idc actually
#pragma warning(disable: 4003)

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

int  Tester::GetI( ) const { return I; }
void Tester::SetI(const int & value) { I = value + 1; }

std::string Tester::GetW1( ) const { return "W1"; }

int main( )
{
	Tester test;

	test.GetA( );
	// No setter
	// test.SetA(123);

	// No getter
	// test.GetB();
	test.SetB(123);

	test.GetC( );
	test.SetC(123);

	test.GetCC( );
	test.SetCC(123);


	test.GetD( );
	test.SetD(123);

	test.GetE( );
	// Private setter
	// test.SetE(123);

	// Private getter
	// test.GetF();
	test.SetF(123);

	// Both getter setter private
	// test.GetG();
	// test.SetG(123);

	test.SetH(123);
	std::cout << test.GetH( ) << std::endl;

	test.SetI(123);
	std::cout << test.GetI( ) << std::endl;

	const int & j = test.GetJ( );
	test.SetJ(j);

	const int * k = test.GetK( );
	test.SetK(k);

	std::string str("str");
	// Private setter
	// test.SetW1(str);
	std::cout << test.GetW1( ) << std::endl;

	std::cout << test.GetW2() << std::endl;

	test.SetW3(1.23);
	std::cout << *test.GetW3() << std::endl;
}

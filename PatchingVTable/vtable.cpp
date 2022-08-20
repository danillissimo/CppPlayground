#include <iostream>

using namespace std;


class A
{
public:
	virtual void print( ) { cout << "Im A\n"; }
};


class B
{
public:
	virtual void print( ) { cout << "Im B\n"; }
};


class VTableViewer
{
public:
	virtual void a( ) { cout << "im v::a\n"; }
	virtual void b( ) { cout << "im v::b\n"; }
	virtual void c( ) { cout << "im v::c\n"; }
	virtual void d( ) { cout << "im v::d\n"; }
	virtual void e( ) { cout << "im v::e\n"; }
};


class VChild : public VTableViewer
{
public:
	virtual void a( ) override { cout << "im vc::a\n"; }
	virtual void b( ) override { cout << "im vc::b\n"; }
	virtual void c( ) override { cout << "im vc::c\n"; }
	virtual void d( ) override { cout << "im vc::d\n"; }
	virtual void e( ) override { cout << "im vc::e\n"; }
};


int main( )
{
	A            a;
	B            b;
	VTableViewer v;

	*((uint64_t *)((char *)(&b))) = *((uint64_t *)((char *)(&v)));

	void (B::*ptr)( ) = &B::print;

	(b.*ptr)( );

	cout << *(uint64_t *)(&v) << endl << endl;

	union u
	{
		void (VChild::*mptr)( );
		int64_t        ptr;
	};

	volatile u hack;
	volatile u h;

	hack.mptr = &VTableViewer::a;
	cout << (hack.ptr) << endl;
	hack.mptr = &VTableViewer::b;
	cout << (hack.ptr) << endl;
	hack.mptr = &VTableViewer::c;
	cout << (hack.ptr) << endl;
	hack.mptr = &VTableViewer::d;
	cout << (hack.ptr) << endl;
	hack.mptr = &VTableViewer::e;
	cout << (hack.ptr) << endl;

	VChild vc;
	hack.mptr = &VTableViewer::c;
	cout << endl << (hack.ptr) << endl;

#if _DEBUG
	union
	{
		uint32_t word;
		char     bytes[4];
	}            offset0;
	offset0.word = *(uint32_t *)((char *)hack.ptr + 1);
	// std::swap(offset0.bytes[0], offset0.bytes[3]);
	// std::swap(offset0.bytes[1], offset0.bytes[2]);
	char * adr = (char *)hack.ptr + offset0.word + 5;

	int offset = *(adr + 4) == 0x60 /* else 0x20 */ ? (int)*(adr + 5) : 0;
	cout << offset << endl;
#else
	int offset = *((char *)hack.ptr + 4) == 0x60 /* else 0x20 */ ? (int)*((char *)hack.ptr + 5) : 0;
	cout << offset << endl;
#endif


	(vc.*(hack.mptr))( );
	uint64_t * hackedTable = new uint64_t[6];
	memcpy(hackedTable, *(char **)(&vc), sizeof(uint64_t) * 6);
	*(uint64_t **)(&vc)                     = hackedTable;
	*(uint64_t *)(*(char **)(&vc) + offset) = *(uint64_t *)(*(char **)(&v) + offset);
	(vc.*(hack.mptr))( );

	h.mptr = &VTableViewer::b;

	return 0;
}

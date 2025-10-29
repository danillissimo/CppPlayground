#ifdef AS_TEXT
R"ESCAPE_SEQ(
#endif

#include <iostream>

const char* getSelfContent()
{
#define AS_TEXT
	return
#include __FILE__
#undef AS_TEXT
}

int main()
{
	std::cout << getSelfContent();
	return 0;
}

#if 0
auto ___ = ")ESCAPE_SEQ";
#endif
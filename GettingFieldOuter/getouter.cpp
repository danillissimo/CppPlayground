#include<iostream>

template<class OwnerType, typename HelperType>
class Field
{
    public:
	OwnerType* GetOuter()
	{
		return (OwnerType*)((char*)this - OwnerType::__GetOffset(HelperType()));
	}
};

#define Field(Outer,Name)\
    /* Such mark doesn't work as it gets aligned by the owning structure beginning */\
    /*[[no_unique_address]] Empty __layout_mark_##__LINE__;*/\
    struct __layout_mark_helper_##__LINE__{};\
    static constexpr size_t __GetOffset(__layout_mark_helper_##__LINE__) { return offsetof(Outer,Name); }\
    friend class Field<Outer,__layout_mark_helper_##__LINE__>;\
    Field<Outer,__layout_mark_helper_##__LINE__> Name

class Holder
{
public:

	int Field1;
	std::string Field2;
	Field(Holder, F);
};

int main(){
    Holder h;
    std::cout << &h.F << " " << h.F.GetOuter();
}
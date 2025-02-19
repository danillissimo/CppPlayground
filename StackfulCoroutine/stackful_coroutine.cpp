#include <iostream>
#include <list>

#define FRAME_PTR ebp
#define STACK_TOP_PTR esp
#define RETURN_VALUE eax
#define REGISTER_THIS ecx
#define REGISTER1 ecx
#define REGISTER2 edx
#define REGISTER3 ebx

#define stack_size (1024 * 1024)

__declspec(noinline, naked) void run_in_another_stack(void (*method)())
{
	__asm {
		// allocate 1 MB aligned by 16 bytes
		push 16
		push stack_size
		call _aligned_malloc
		add STACK_TOP_PTR, 8

		// move pointer to the top of the new stack
		add RETURN_VALUE, stack_size

		// cache stuff
		mov REGISTER1, [STACK_TOP_PTR + 4]// cache method ptr
		mov REGISTER2, FRAME_PTR          // cache frame pointer
		mov REGISTER3, STACK_TOP_PTR      // cache stack top pointer

		// assign new stack
		mov FRAME_PTR, RETURN_VALUE
		mov STACK_TOP_PTR, RETURN_VALUE

		// cache old values there
		push REGISTER2
		push REGISTER3

		// call provided method
		call REGISTER1

		// retrieve old values back from stack
		pop REGISTER3
		pop REGISTER2

		// cache new stack ptr to deallocate it later
		mov REGISTER1, FRAME_PTR

		// restore old stack
		mov STACK_TOP_PTR, REGISTER3
		mov FRAME_PTR, REGISTER2

		// move new stack pointer back to the beginning and deallocate it
		sub REGISTER1, stack_size
		push REGISTER1
		call _aligned_free

		// done
		add STACK_TOP_PTR, 4
		ret
		}
}

struct return_point
{
	// don't forget stack is reversed - data pushed first has a greater address then the one pushed later
	void* frame_ptr;
	void* stack_top;
	void* return_address;
};

static_assert(sizeof(return_point) == 12, "");

std::list<return_point> return_points;

void add_return_point(return_point&& point)
{
	return_points.push_back(point);
}

return_point& get_first_return_point()
{
	return return_points.front();
}

void drop_first_point()
{
	return_points.pop_front();
}

__declspec(noinline, naked) void get_yield_target()
{
	__asm {
		// cache the point to yield to
		call get_first_return_point
		push dword ptr[RETURN_VALUE]    // frame pointer
		push dword ptr[RETURN_VALUE + 4]// stack top
		push dword ptr[RETURN_VALUE + 8]// jump target

		// remove it once its cached
		call drop_first_point

		// retrieve the point to yield to
		pop REGISTER3// jump target
		pop REGISTER2// stack top
		pop REGISTER1// frame pointer

		ret
		}
}

__declspec(noinline, naked) void yield()
{
	__asm{
		// check if there's anywhere to yield
		mov REGISTER_THIS, offset return_points
		call std::list<return_point, std::allocator<return_point>>::size
		cmp eax,0
		jne can_yield
		ret

		// gather data to be added to return point list
		// return address is already in the stack
		// fix stack top address as if it was after return
		can_yield:
		mov REGISTER1, STACK_TOP_PTR
		add REGISTER1, 4
		push REGISTER1
		push FRAME_PTR

		// add data to the list
		lea REGISTER1, [STACK_TOP_PTR]
		push REGISTER1
		call add_return_point
		add STACK_TOP_PTR, 16// drop all pushed stuff including return address - its not needed anymore

		// get the point to yield to
		call get_yield_target

		// apply it
		mov STACK_TOP_PTR, REGISTER2
		mov FRAME_PTR, REGISTER1
		jmp REGISTER3
		}
}

__declspec(noinline, naked) void go(void (*method)())
{
	__asm {
		// gather data to be added to return point list
		// return address is already in the stack
		// fix stack top address as if it was after return
		mov REGISTER1, STACK_TOP_PTR
		add REGISTER1, 4
		push REGISTER1
		push FRAME_PTR

		// add data to the list
		lea REGISTER1, [STACK_TOP_PTR]
		push REGISTER1
		call add_return_point
		add STACK_TOP_PTR, 16// drop all pushed stuff including return address - its not needed anymore

		// allocate 1 MB aligned by 16 bytes
		push 16
		push stack_size
		call _aligned_malloc
		add STACK_TOP_PTR, 8

		// move pointer to the top of the new stack
		add RETURN_VALUE, stack_size

		// retrieve method pointer from stack while its there
		pop REGISTER1

		// assign new stack
		mov FRAME_PTR, RETURN_VALUE
		mov STACK_TOP_PTR, RETURN_VALUE

		// call provided method
		call REGISTER1

		// move new stack pointer back to the beginning and cache it to deallocate later
		mov REGISTER1, FRAME_PTR
		sub REGISTER1, stack_size
		push REGISTER1

		// try to yield
		mov REGISTER_THIS, offset return_points
		call std::list<return_point, std::allocator<return_point>>::size
		cmp eax, 0
		je cant_yield

		call get_yield_target       // get yield target
		pop RETURN_VALUE            // cache new stack address
		mov FRAME_PTR, REGISTER1    // apply new stack frame
		mov STACK_TOP_PTR, REGISTER2// apply new stack top
		push REGISTER3              // cache jump target

		// deallocate new stack
		push RETURN_VALUE
		call _aligned_free
		add STACK_TOP_PTR, 4

		// jump to the new target
		pop REGISTER3
		jmp REGISTER3


		// exit program if nowhere to yield
		// dont deallocate this stack as its still needed to do stuff
		cant_yield:
		push 0
		call exit
		int 3
		}
}

void nested_call3()
{
	std::cout << "nested3\n";
}

void nested_call2()
{
	std::cout << "nested2\n";
	yield();
	nested_call3();
}

void nested_call1()
{
	std::cout << "nested1\n";
	yield();
	nested_call2();
}

void print()
{
	std::cout << "hey\n";
	nested_call1();
	yield();
}

void do_other_stuff()
{
	std::cout << "other stuff\n";
	yield();
}

int main()
{
	go(&print);
	std::cout << "hello there\n";
	yield();
	do_other_stuff();
	std::cout << "done\n";
	return 0;
}
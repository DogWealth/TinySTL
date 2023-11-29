#include "TinyArray.h"
#include "TinyVector.h"

#include <vector>


//int在一些编译器是16位，也可能32位。
//只有int32能保证32位
void InitFun(TinySTL::Array<int32_t, 33>& a)
{

	/*
	c++11会查找begin()和end()
	等价于
	for(auto _it = a.begin(), auto& it = *_it; it != a.end(); ++it)
	*/
	for (auto& it : a)
	{
		it = 0;
	}
}

class A
{
public:
	int* x;

	A()
	{
		x = new int();
		std::cout << "construct: x's value is " << *x << "\nAnd x's address is " << x << std::endl;
	}

	A(int* p)
	{
		x = new(p) int(996);
		std::cout << "construct: x's value is " << *x << "\nAnd x's address is " << x << std::endl;
	}

	~A()
	{
		std::cout << "destroy: " << x << std::endl;
	}
};

int main()
{
	/*using alloc = std::allocator<A>;
	A* p1 = alloc{}.allocate(1);
	int* p2 = std::allocator<int>{}.allocate(1);
	std::cout << "A address: " << p1 << std::endl;
	std::cout << "x address: " << p2 << std::endl;

	std::construct_at(p1, p2);
	std::construct_at(p1);

	std::cout << "check out the int num in " << p2 << " is " << *p2 << std::endl;*/


	TinySTL::Array<int32_t, 33> array;
	InitFun(array);
	

	TinySTL::Vector<int> a;
	std::vector<int> b;
	double start, end, cost;

	/*push_back*/
	std::cout << "push_back：100000 elements" << "\n";
	start = clock();
	for (int i = 0; i < 100000; i++)
		a.push_back(i);
	end = clock();
	std::cout << "TinyVector push_back time: " << end - start << std::endl;

	start = clock();
	for (int i = 0; i < 100000; i++)
		b.push_back(i);
	end = clock();
	std::cout << "STLVector push_back time: " << end - start << std::endl;


	/*erase*/
	std::cout << "erase：1900 elements" << "\n";
	start = clock();
	for (int i = 0; i < 1900; i++)
		a.erase(a.begin());
	end = clock();
	std::cout << "TinyVector erase time: " << end - start << std::endl;

	start = clock();
	for (int i = 0; i < 1900; i++)
		b.erase(b.begin());
	end = clock();
	std::cout << "STLVector erase time: " << end - start << std::endl;

	TinySTL::Vector<int>  c = a;

	/*insert*/
	std::cout << "insert：100 elements" << "\n";
	start = clock();
	for (int i = 0; i < 100; i++)
		a.insert(a.begin(), c.begin(), c.end());
	end = clock();
	std::cout << "TinyVector insert time: " << end - start << std::endl;

	start = clock();
	for (int i = 0; i < 100; i++)
		b.insert(b.begin(), c.begin(), c.end());
	end = clock();
	std::cout << "STLVector insert time: " << end - start << std::endl;
	
}
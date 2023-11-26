#include "TinyArray.h"



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

int main()
{
	TinySTL::Array<int32_t, 33> array;
	InitFun(array);
	for (size_t i = 0; i < array.size(); i++)
		std::cout << array[i] << '\n';

	for (size_t i = 0; i < array.size(); i++)
		std::cout << array.at(i) << '\n';
}
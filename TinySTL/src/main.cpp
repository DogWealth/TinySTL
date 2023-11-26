#include "TinyArray.h"



//int��һЩ��������16λ��Ҳ����32λ��
//ֻ��int32�ܱ�֤32λ
void InitFun(TinySTL::Array<int32_t, 33>& a)
{

	/*
	c++11�����begin()��end()
	�ȼ���
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
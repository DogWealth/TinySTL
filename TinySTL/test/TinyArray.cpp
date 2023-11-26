#include <iostream>

namespace TinySTL {
	/*size_t可以在不同平台上有不同的大小，肯定比int大，超过两亿的数据就没法用int去遍历*/
	template <class T, size_t N>
	struct Array
	{
		using value_type = T;
		using iterator = T*;
		using const_iterator = const T*;

		/*
		[]中括号下标索引
		如果数组对象是 const 限定方法，则返回 const 引用，否则返回引用。
		*/
		T& operator[](size_t i) noexcept
		{
			return m_Elements[i];
		}

		const T& operator[](size_t i) const noexcept
		{
			return m_Elements[i];
		}



		/*
		at()索引，
		如果 N 是有效索引，则返回给定数组中索引 N 处的元素，否则抛出 out_of_range 异常。
		如果数组对象是 const 限定方法，则返回 const 引用，否则返回引用。
		*/
		T& at(size_t i)
		{
			if (i < 0 || i >= N) throw std::runtime_error("out of range!");
			return m_Elements[i];
		}

		const T& at(size_t i) const
		{
			if (i < 0 || i >= N) throw std::runtime_error("out of range!");
			return m_Elements[i];
		}



		/*
		获取array大小,
		不依赖实例
		*/
		static constexpr size_t size() noexcept
		{
			return N;
		}

		static constexpr size_t max_size() noexcept
		{
			return N;
		}



		/*
		迭代器
		RandomAccessIterator
		++p
		--p
		p += n, p + n
		p -= n, p - n
		*p
		p->
		p[n]
		p1 - p2
		p1 != p2
		p1 < p2
		*/

		T* begin() noexcept
		{
			return m_Elements;
		}

		T* end() noexcept
		{
			return m_Elements + N;
		}

		const T* begin() const noexcept
		{
			return m_Elements;
		}

		const T* end() const noexcept
		{
			return m_Elements + N;
		}

		const T* cbegin() const noexcept
		{
			return m_Elements;
		}

		const T* cend() const noexcept
		{
			return m_Elements + N;
		}

		/*
		data
		*/
		T* data() noexcept
		{
			return m_Elements;
		}

		const T* data() const noexcept
		{
			return m_Elements;
		}

		const T* cdata() const noexcept
		{
			return m_Elements;
		}

		
		/*
		front()
		back()
		*/
		T& front() noexcept
		{
			return m_Elements[0];
		}

		const T& front() const noexcept
		{
			return m_Elements[0];
		}

		T& back() noexcept
		{
			return m_Elements[N - 1];
		}

		const T& back() const noexcept
		{
			return m_Elements[N - 1];
		}


		/*
		fill()填充数组
		两层noexcept
		如果拷贝无异常noexcept(T(val))返回true
		noexcept(true)表示不抛出异常
		否则noexcept(false)相当于noexcept无效
		*/
		void fill(const T& val) noexcept(noexcept(T(val)))
		{
			for (size_t i = 0; i < N; i++)
			{
				m_Elements[i] = val;
			}
		}


	private:
		T m_Elements[N];
	};


	/*特例化一个长度0*/
	template <class T>
	struct Array<T, 0>
	{
		using value_type = T;
		using iterator = T*;
		using const_iterator = const T*;

		T& operator[](size_t i)
		{
			throw std::runtime_error("out of range!");
		}

		const T& operator[](size_t i)
		{
			throw std::runtime_error("out of range!");
		}


		T& at(size_t i)
		{
			throw std::runtime_error("out of range!");
		}

		const T& at(size_t i) const
		{
			throw std::runtime_error("out of range!");
		}


		static constexpr size_t size() noexcept
		{
			return 0;
		}

		static constexpr size_t max_size() noexcept
		{
			return 0;
		}

		T* begin() noexcept
		{
			return nullptr;
		}

		T* end() noexcept
		{
			return nullptr;
		}

		const T* begin() const noexcept
		{
			return nullptr;
		}

		const T* end() const noexcept
		{
			return nullptr;
		}

		const T* cbegin() const noexcept
		{
			return nullptr;
		}

		const T* cend() const noexcept
		{
			return nullptr;
		}

		T* data() noexcept
		{
			return nullptr;
		}

		const T* data() const noexcept
		{
			return nullptr;
		}

		const T* cdata() const noexcept
		{
			return nullptr;
		}
	}; 
}


//int在一些编译器是16位，也可能32位。
//只有int32能保证32位
//
//void InitFun(TinySTL::Array<int32_t, 33>& a)
//{
//
//	/*
//	c++11会查找begin()和end()
//	等价于
//	for(auto _it = a.begin(), auto& it = *_it; it != a.end(); ++it)
//	*/
//	for (auto &it : a)
//	{
//		it = 0;
//	}
//}
//
//int main()
//{
//	TinySTL::Array<int32_t, 33> array;
//	InitFun(array);
//	for (size_t i = 0; i < array.size(); i++)
//		std::cout << array[i] << '\n';
//
//	for (size_t i = 0; i < array.size(); i++)
//		std::cout << array.at(i) << '\n';
//}
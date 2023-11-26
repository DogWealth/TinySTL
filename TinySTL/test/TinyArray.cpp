#include <iostream>

namespace TinySTL {
	/*size_t�����ڲ�ͬƽ̨���в�ͬ�Ĵ�С���϶���int�󣬳������ڵ����ݾ�û����intȥ����*/
	template <class T, size_t N>
	struct Array
	{
		using value_type = T;
		using iterator = T*;
		using const_iterator = const T*;

		/*
		[]�������±�����
		������������ const �޶��������򷵻� const ���ã����򷵻����á�
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
		at()������
		��� N ����Ч�������򷵻ظ������������� N ����Ԫ�أ������׳� out_of_range �쳣��
		������������ const �޶��������򷵻� const ���ã����򷵻����á�
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
		��ȡarray��С,
		������ʵ��
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
		������
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
		fill()�������
		����noexcept
		����������쳣noexcept(T(val))����true
		noexcept(true)��ʾ���׳��쳣
		����noexcept(false)�൱��noexcept��Ч
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


	/*������һ������0*/
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


//int��һЩ��������16λ��Ҳ����32λ��
//ֻ��int32�ܱ�֤32λ
//
//void InitFun(TinySTL::Array<int32_t, 33>& a)
//{
//
//	/*
//	c++11�����begin()��end()
//	�ȼ���
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
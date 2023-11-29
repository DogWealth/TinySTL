#pragma once
#include <iostream>
#include <cstring>
#include <iterator>
#include <concepts>

namespace TinySTL {
	template <class T, class Alloc = std::allocator<T>>
	struct Vector
	{
		using value_type = T;
		using pointer = T*;
		using const_pointer = const T*;
		using reference = T&;
		using const_reference = const T&;
		using iterator = T*;
		using const_iterator = const T*;
		using reverse_iterator = std::reverse_iterator<T*>;
		using const_reverse_iterator = std::reverse_iterator<const T*>;
		using allocator = Alloc;

		int* m_data;
		size_t m_size;
		size_t m_cap;


		Vector() noexcept
		{
			m_data = nullptr;
			m_size = 0;
			m_cap = 0;
		}

		Vector(std::initializer_list<T> ilist)
			: Vector(ilist.begin(), ilist.end())
		{
		}

		template<std::random_access_iterator InputIt>
		Vector(InputIt first, InputIt last)
		{
			size_t n = last - first;
			reserve(n);
			m_size = n;
			for (size_t i = 0; i < n; i++)
			{
				m_data[i] = *first;
				++first;
			}
		}

		explicit Vector(size_t n)
		{
			m_data = allocator{}.allocate(n);
			for (size_t i = 0; i < n; i++)
			{
				std::construct_at(&m_data[i]);
			}
			m_size = n;
			m_cap = n;
		}

		explicit Vector(size_t n, const T& val)
		{
			m_data = allocator{}.allocate(n);
			for (size_t i = 0; i < n; i++)
			{
				std::construct_at(&m_data[i], val);
			}
			m_size = n;
			m_cap = n;
		}

		/*
		析构函数里释放内存，一定要写一个拷贝构造函数，否则浅拷贝会造成内存的double free
		*/
		Vector(const Vector& that)
		{
			m_cap = m_size = that.m_size;
			if (m_size != 0)
			{
				/*
				表达式new和delete把内存分配和构造以及析构耦合在一起。
				在这里用allocator和construct_at（placement new）来解耦
				如果直接new。自定义类会调用默认构造函数初始化，会有开销
				*/
				m_data = allocator{}.allocate(m_size);
				for (size_t i = 0; i < m_size; i++)
				{
					/*
					as_const自定义类会调用拷贝构造函数，std::move会调用移动构造
					这里等价于placement_new:
					new (&m_data[i]) T (std::as_const(that.m_data[i]))
					*/

					std::construct_at(&m_data[i], std::as_const(that.m_data[i]));
				}
				
			}
			else
			{
				m_data = nullptr;
			}
		}

		/*
		移动构造函数
		不会申请新内存，不会抛出异常
		*/

		Vector(Vector&& that) noexcept
		{
			m_data = that.m_data;
			m_size = that.m_size;
			m_cap = that.m_cap;

			that.m_data = nullptr;
			that.m_size = 0;

		}

		/*
		拷贝赋值和拷贝构造类似但有细节上的不同
		如果不单独写拷贝赋值也会出现同拷贝构造一样的问题
		注意右边如果存在数据要删去
		*/
		Vector& operator=(const Vector& that)
		{
			clear();
			reserve(that.m_size);
			m_size = that.m_size;
			if (m_size != 0)
			{
				for (size_t i = 0; i < m_size; i++)
				{
					std::construct_at(&m_data[i], std::as_const(that.m_data[i]));
				}
			}
			return *this;
		}

		Vector& operator=(Vector&& that) noexcept
		{
			if (that == this) return *this;

			for (size_t i = 0; i != m_size; i++) 
			{
				std::destroy_at(&m_data[i]);
			}

			if (m_cap)
			{
				allocator{}.deallocate(m_data, m_cap);
			}

			m_data = that.m_data;
			m_size = that.m_size;
			m_cap = that.m_cap;

			that.m_data = nullptr;
			that.m_size = 0;

			return *this;
		}

		void assign(std::initializer_list<T> ilist)
		{
			assign(ilist.begin(), ilist.end());
		}

		template<std::random_access_iterator InputIt>
		void assign(InputIt first, InputIt last)
		{
			clear();
			size_t n = last - first;
			reserve(n);
			m_size = n;

			for (size_t i = 0; i < n; i++)
			{
				std::construct_at(&m_data[i], *first);;
				++first;
			}
		}

		void clear()
		{

			resize(0);
			m_data = nullptr;
		}

		//resize
		void resize(size_t n)
		{
			reserve(n);
			if (n > m_size)
			{
				for (size_t i = m_size; i < n; i++)
					std::construct_at(&m_data[i]);
			}
			m_size = n;
		}

		void resize(size_t n, T val)
		{
			if (n < m_size)
			{
				for (size_t i = n; i != m_size; i++)
				{
					std::destroy_at(&m_data[i]);
				}
			}
			else if (n > m_size)
			{
				reserve(n);
				for (size_t i = m_size; i < n; i++)
					std::construct_at(&m_data[i], val);
			}
			m_size = n;
		}

		/*
		防止内存的浪费可以缩小capacity
		*/
		void shrink_to_fit() noexcept
		{
			auto old_data = m_data;
			auto old_cap = m_cap;
			m_cap = m_size;

			if (m_size == 0)
			{
				m_data = nullptr;
			}
			else
			{
				m_data = allocator{}.allocate(m_size);
			}

			if (old_data)
			{
				if (m_size)
					//memcpy(m_data, old_data, m_size * sizeof(int));
					//这里用memcpy对于自定义类型，指针会被后面的delete[] old_data释放掉
					for (size_t i = 0; i < m_size; i++)
					{
						std::construct_at(&m_data[i], std::as_const(old_data[i]));
						std::destroy_at(&old_data[i]);
					}

				//delete[] old_data;
				allocator{}.deallocate(old_data, old_cap);

			}
		}

		void reserve(size_t n)
		{
			/*
			* 
			vector的扩容机制是：
			当目前可用的空间不足时，分配目前空间的两倍或者目前空间加上所需的新空间大小（取较大值），
			容量的扩张必须经过“重新配置、元素移动、释放原空间”等过程。

			memcpy：
			dest	-	pointer to the memory location to copy to
			src	-	pointer to the memory location to copy from
			count	-	number of bytes to copy
			*/
			if (n <= m_cap) return;
			n = std::max(n, m_cap * 2);

			//printf("grow from %zd to %zd\n", m_cap, n);

			auto old_data = m_data;
			auto old_cap = m_cap;
			
			m_data = allocator{}.allocate(n);
			m_cap = n;
			
			if (old_data)
			{
				if(m_size) 
					for (size_t i = 0; i < m_size; i++)
					{
						std::construct_at(&m_data[i], std::as_const(old_data[i]));
						std::destroy_at(&old_data[i]);
					}

				allocator{}.deallocate(old_data, old_cap);
			}
		}

		constexpr size_t size() const noexcept
		{
			return m_size;
		}

		constexpr size_t capacity() const noexcept
		{
			return m_cap;
		}

		T* data() noexcept
		{
			return m_data;
		}

		const T* data() const noexcept
		{
			return m_data;
		}

		const T& operator[] (size_t i) const noexcept
		{
			return m_data[i];
		}

		T& operator[] (size_t i) noexcept
		{
			return m_data[i];
		}

		T& front() noexcept
		{
			return operator[](0);
		}

		const T& front() const noexcept
		{
			return operator[](0);
		}

		T& back() noexcept
		{
			return operator[](size() - 1);
		}

		const T& back() const noexcept
		{
			return operator[](size() - 1);
		}

		//push_back

		void push_back(const T& value)
		{
			//resize(m_size + 1);因为后面初始化还要赋值，效率变低了用reserve
			reserve(m_size + 1);
			std::construct_at(&m_data[m_size], value);
			m_size++;
		}

		//移动
		void push_back(T&& value)
		{
			//resize(m_size + 1);因为后面初始化还要赋值，效率变低了用reserve
			reserve(m_size + 1);
			std::construct_at(&m_data[m_size], std::move(value));
			m_size++;
		}

		//emplace_back
		/*
		emplace_back可以直接用参数在容器内部构造
		相比于pushi_back可以免去一次传入参数的拷贝构造/移动构造

		万能引用：万能引用又被叫做转发引用，他既可能是左值引用，又可能是右值引用。 当满足以下两种情况时，此时属于万能引用：
		函数参数是T&&, 且T是这个函数模板的模板类型
		当我们入参传入左值时，他就是个左值引用；当我们入参传入右值时，他就是个右值引用
		*/
		template<class ...Args>
		T& emplace_back(Args&& ...args)
		{
			reserve(m_size + 1);
			T* p = &m_data[m_size];
			std::construct_at(p, std::forward<Args>(args)...);
			m_size++;
			return *p;
		}

		/*
		擦除
		*/
		void erase(size_t i)
		{
			for (size_t j = i + 1; j < m_size; j++)
			{
				std::destroy_at(&m_data[j - 1]);
				m_data[j - 1] = std::move(m_data[j]);
			}
			resize(m_size - 1);
		}

		void erase(size_t ibeg, size_t iend)
		{
			size_t diff = iend - ibeg + 1;
			for (size_t j = iend + 1; j < m_size; j++)
			{
				std::destroy_at(&m_data[j - diff]);
				m_data[j - diff] = std::move(m_data[j]);
			}
			resize(m_size - diff);
		}

		T* begin()
		{
			return m_data;
		}

		T* end()
		{
			return m_data + m_size;
		}

		const T* begin() const
		{
			return m_data;
		}

		const T* end() const
		{
			return m_data + m_size;
		}

		/*
		迭代器版本erase
		*/

		void erase(const T* it)
		{
			size_t i = it - m_data;
			for (size_t j = i + 1; j < m_size; j++)
			{
				std::destroy_at(&m_data[j - 1]);
				m_data[j - 1] = std::move(m_data[j]);
			}
			resize(m_size - 1);
		}

		void erase(const T* first, const T* last)
		{
			size_t diff = last - first;
			for (size_t j = last - m_data + 1; j < m_size; j++)
			{
				std::destroy_at(&m_data[j - diff]);
				m_data[j - diff] = std::move(m_data[j]);
			}
			resize(m_size - diff);
		}

		/*
		insert
		插入n个相同val版本
		迭代器版本
		*/
		void insert(T const* it, size_t n, const T& val)
		{
			size_t j = it - m_data;

			if (n == 0) return;

			reserve(m_size + n);
			m_size += n;

			for (size_t i = n; i >= 0; i++)
			{
				std::construct_at(m_data[j + n + i - 1], std::move(m_data[j + i - 1]));
				std::destroy_at(&m_data[j + i - 1]);
			}

			for (size_t i = j; i < n; i++)
			{
				std::construct_at(m_data[j + n + i - 1], val);
			}
		}


		template<std::random_access_iterator InputIt>
		void insert(T const* it, InputIt first, InputIt last)
		{
			size_t j = it - m_data;
			size_t n = last - first;
			if (n == 0) return;

			/*
			注意reserve之后因为new了新的data，it，first和last地址全部无效了
			所以如果first和last是自身的迭代器程序就crush了
			但是STL的vector可以这么操作，可能是建了一个新的vector
			*/
			reserve(m_size + n);
			m_size += n;

			for (size_t i = 0; i <n; i++)
			{
				std::construct_at(&m_data[j + n + i - 1], std::move(m_data[j + i - 1]));
				std::destroy_at(&m_data[j + i - 1]);
			}

			for (size_t i = j; i < n; i++)
			{
				std::construct_at(&m_data[j + n + i - 1], *first);
				++first;
			}
		}

		void insert(T const* it, std::initializer_list<T> ilist)
		{
			insert(it, ilist.begin(), ilist.end());
		}


		~Vector() noexcept
		{
			allocator{}.deallocate(m_data, m_cap);
		}

	};
}
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
		�����������ͷ��ڴ棬һ��Ҫдһ���������캯��������ǳ����������ڴ��double free
		*/
		Vector(const Vector& that)
		{
			m_cap = m_size = that.m_size;
			if (m_size != 0)
			{
				/*
				���ʽnew��delete���ڴ����͹����Լ����������һ��
				��������allocator��construct_at��placement new��������
				���ֱ��new���Զ���������Ĭ�Ϲ��캯����ʼ�������п���
				*/
				m_data = allocator{}.allocate(m_size);
				for (size_t i = 0; i < m_size; i++)
				{
					/*
					as_const�Զ��������ÿ������캯����std::move������ƶ�����
					����ȼ���placement_new:
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
		�ƶ����캯��
		�����������ڴ棬�����׳��쳣
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
		������ֵ�Ϳ����������Ƶ���ϸ���ϵĲ�ͬ
		���������д������ֵҲ�����ͬ��������һ��������
		ע���ұ������������Ҫɾȥ
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
		��ֹ�ڴ���˷ѿ�����Сcapacity
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
					//������memcpy�����Զ������ͣ�ָ��ᱻ�����delete[] old_data�ͷŵ�
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
			vector�����ݻ����ǣ�
			��Ŀǰ���õĿռ䲻��ʱ������Ŀǰ�ռ����������Ŀǰ�ռ����������¿ռ��С��ȡ�ϴ�ֵ����
			���������ű��뾭�����������á�Ԫ���ƶ����ͷ�ԭ�ռ䡱�ȹ��̡�

			memcpy��
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
			//resize(m_size + 1);��Ϊ�����ʼ����Ҫ��ֵ��Ч�ʱ������reserve
			reserve(m_size + 1);
			std::construct_at(&m_data[m_size], value);
			m_size++;
		}

		//�ƶ�
		void push_back(T&& value)
		{
			//resize(m_size + 1);��Ϊ�����ʼ����Ҫ��ֵ��Ч�ʱ������reserve
			reserve(m_size + 1);
			std::construct_at(&m_data[m_size], std::move(value));
			m_size++;
		}

		//emplace_back
		/*
		emplace_back����ֱ���ò����������ڲ�����
		�����pushi_back������ȥһ�δ�������Ŀ�������/�ƶ�����

		�������ã����������ֱ�����ת�����ã����ȿ�������ֵ���ã��ֿ�������ֵ���á� �����������������ʱ����ʱ�����������ã�
		����������T&&, ��T���������ģ���ģ������
		��������δ�����ֵʱ�������Ǹ���ֵ���ã���������δ�����ֵʱ�������Ǹ���ֵ����
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
		����
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
		�������汾erase
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
		����n����ͬval�汾
		�������汾
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
			ע��reserve֮����Ϊnew���µ�data��it��first��last��ַȫ����Ч��
			�������first��last������ĵ����������crush��
			����STL��vector������ô�����������ǽ���һ���µ�vector
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
#pragma once
#include <type_traits>
#include <array>
#include <cassert>
#include <cstdint>

namespace end
{
	template<typename T, int16_t N>
	class Sorted_Pool_t
	{
	public:
		// Todo: Implement the function bodies

		// Returns the number of active elements
		size_t size()const { return this->active_count; }

		// Returns the maximum supported number of elements 
		size_t capacity()const { return N; }

		// Returns the value at the specified index
		T& operator[](int16_t index) { return this->pool[index]; }

		// Returns the value at the specified index
		const T& operator[](int16_t index)const { return this->pool[index]; }

		// Returns the index of the first inactive element 
		//   and updates the active count
		// Returns -1 if no inactive elements remain
		int16_t alloc() 
		{ 
			if (this->active_count == N)
				return -1;
			else
				return this->active_count += 1; 
		}

		// Moves the element at 'index' to the inactive
		// region and updates the active count
		void free(int16_t index)
		{
			std::swap(this->pool[index], this->pool[--active_count]);
			//--active_count;
			//auto temp = this->pool[index];
			//this->pool[index] = this->pool[active_count];
			//this->pool[active_count] = temp;
		}

	private:

		T pool[N];

		int16_t active_count = 0;
	};

	template<typename T, int16_t N>
	class Pool_t
	{
		public:
		// Todo: Implement the function bodies

		// Removes the first element from the free list and returns its index
		// Returns -1 if no free elements remain
		int16_t alloc()
		{
			if (this->free_start == -1)
				return -1;

			int index = this->free_start;
			this->free_start = this->pool[index].next;
			return index;
		}

		// Adds 'index' to the free list
		// sets index.next = currHead
		// then sets currHead = index
		void free(int16_t index)
		{
			this->pool[index].next = this->free_start;
			this->free_start = index;
		}

		// Initializes the free list
		Pool_t()
		{

			for (int16_t i = 0; i < N; ++i)
			{
				this->pool[i].value = T();
				this->pool[i].next = i + 1;
			}
			this->pool[N - 1].next = -1;
		}

		// Returns the value at the specified index
		T& operator[](int16_t index) { return this->pool[index].value; }

		// Returns the value at the specified index
		const T& operator[](int16_t index)const { return this->pool[index].value; }

	private:

		union element_t
		{
			T value;
			int16_t next;
			
			element_t() : next(0) {}
			~element_t() {}
		};

		element_t pool[N];

		int16_t free_start = 0;
	};
}
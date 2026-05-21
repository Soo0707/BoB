#ifndef BOB_SPARSE SET
#define BOB_SPARSE_SET

#include <type_traits>
#include <memory>
#include <cstddef>
#include <cstdint>

namespace bob
{
	template <typename T>
	class sparse_set
	{
		public:
			sparse_set();
			~sparse_set()
			{

			}

			sparse_set(const sparse_set&) = delete;
			sparse_set(sparse_set&&) = delete; // allow move if noexcept

			sparse_set& operator=(const sparse_set&) = delete;
			sparse_set& operator=(sparse_set&&) = delete;

			size_t size() const noexcept
			{
				return this->m_Size;
			}



		private:
			size_t m_Size;
			size_t m_Capacity;

			uint32_t* m_SparseBuffer;
			T* m_DenseBuffer;

			std::allocator<uint32_t> m_SparseAllocator;
			std::allocator<T> m_DenseAllocator;
	};
};

#endif

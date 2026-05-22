#ifndef BOB_SPARSE_SET
#define BOB_SPARSE_SET

#include <type_traits>
#include <memory>
#include <cstddef>
#include <cstdint>
#include <string>

#include "entity_handle.hpp"

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
			sparse_set(sparse_set&&) = delete; // allow noexcept move

			sparse_set& operator=(const sparse_set&) = delete;
			sparse_set& operator=(sparse_set&&) = delete;

			size_t sparse_size() const noexcept
			{
				return this->m_SparseSize;
			}

			size_t dense_size() const noexcept
			{
				return this->m_DenseSize;
			}

			const uint32_t& sparse() const noexcept
			{
				return *this->m_SparseBuffer;
			}

			const bob::entity_handle& handles() const noexcept
			{
				return *this->m_HandleBuffer;
			}

			const T& components() const noexcept
			{
				return *this->m_ComponentBuffer;
			}

			void extend_sparse(const size_t new_size) noexcept
			{
				if (new_size < this->m_SparseSize)
					return;

				uint32_t* new_sparse_buffer = this->m_IndexAllocator.allocate(new_size);
				std::memcpy(new_sparse_buffer, this->m_SparseBuffer, this->m_SparseSize * sizeof(uint32_t));
				this->m_IndexAllocator.deallocate(this->m_SparseBuffer, this->m_SparseSize);

				for (size_t i = this->m_SparseSize; i < new_size; i++)
					new_sparse_buffer[i] = bob::null_index;

				this->m_SparseBuffer = new_sparse_buffer;
				this->m_SparseSize = new_size;
			}

			template <typename... Arg>
			void add(const bob::entity_handle handle, Arg&&... args) noexcept
			{
				if (this->m_DenseSize + 1 > this->m_DenseCapacity)
				{

				}

				new (this->m_SparseBuffer + handle.index()) uint32_t(this->m_DenseSize);
				new (this->m_HandleBuffer + this->m_DenseSize) bob::entity_handle(handle);
				new (this->m_ComponentBuffer + this->m_DenseSize) T(std::forward<Arg>(args)...);

				this->m_DenseSize++;
			}

			void remove(const bob::entity_handle handle) noexcept
			{

			}

		private:
			// sparse size and capacity will always be the same
			size_t m_SparseSize;

			size_t m_DenseSize;
			size_t m_DenseCapacity;

			uint32_t* m_SparseBuffer;

			bob::entity_handle* m_HandleBuffer;
			T* m_ComponentBuffer;

			std::allocator<uint32_t> m_IndexAllocator;
			std::allocator<bob::entity_handle> m_HandleAllocator;
			std::allocator<T> m_ComponentAllocator;
	};
};
#endif

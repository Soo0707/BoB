#ifndef BOB_SPARSE_SET
#define BOB_SPARSE_SET

#include <type_traits>
#include <memory>
#include <cstddef>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <algorithm>

#include "entity_handle.hpp"

namespace bob
{
	struct handle_proxy
	{
		size_t size;
		const entity_handle* handles;
	};

	template <typename T>
	struct component_proxy
	{
		size_t size;
		T* components;
	};

	struct abstract_sparse_set
	{
		abstract_sparse_set() = default;
		virtual ~abstract_sparse_set() = default;
	};
	// TODO: revert back to std::vector but make a custom allocator
	template <typename T>
	class sparse_set : public abstract_sparse_set
	{
		public:
			sparse_set() :
				abstract_sparse_set(),
				m_SparseBuffer(nullptr),
				m_HandleBuffer(nullptr),
				m_ComponentBuffer(nullptr),
				m_SparseSize(0),
				m_DenseSize(0),
				m_DenseCapacity(0)
			{}

			~sparse_set() override
			{
				if constexpr (!std::is_trivially_destructible_v<T>)
				{
					for (size_t i = 0, n = this->m_DenseSize; i < n; i++)
						this->m_ComponentBuffer[i].~T();
				}

				this->m_IndexAllocator.deallocate(this->m_SparseBuffer, this->m_SparseSize);
				this->m_HandleAllocator.deallocate(this->m_HandleBuffer, this->m_DenseCapacity);
				this->m_ComponentAllocator.deallocate(this->m_ComponentBuffer, this->m_DenseCapacity);
			}

			sparse_set(const sparse_set&) = delete;
			sparse_set(sparse_set&&) = delete; // allow noexcept move

			sparse_set& operator=(const sparse_set&) = delete;
			sparse_set& operator=(sparse_set&&) = delete;

			handle_proxy get_handles() const noexcept
			{
				assert(
						this->m_DenseSize != 0 &&
						"BOB [sparse_set][get_handles()]: cannot return reference to entity handles when dense size is 0"
						);
				return handle_proxy { this->m_DenseSize, this->m_HandleBuffer };
			}

			component_proxy<T> get_components() const noexcept
			{
				assert(
						this->m_DenseSize != 0 &&
						"BOB [sparse_set][get_components()]: cannot return reference to components when dense size is 0"
						);
				return component_proxy<T>{ this->m_DenseSize, this->m_ComponentBuffer };
			}

			template <typename... Arg>
			void add(const entity_handle handle, Arg&&... args) noexcept
			{
				if (handle.index() >= this->m_SparseSize)
					this->m_ExtendSparseBuffer(handle.index() + 1);
				
				if (this->m_DenseSize + 1 > this->m_DenseCapacity)
				{
					const size_t new_capacity = (this->m_DenseCapacity == 0) ? 16 : 2 * this->m_DenseCapacity;
					this->m_ExtendHandleBuffer(new_capacity);
					this->m_ExtendComponentBuffer(new_capacity);
					this->m_DenseCapacity = new_capacity;
				}

				new (this->m_SparseBuffer + handle.index()) uint32_t(this->m_DenseSize);
				new (this->m_HandleBuffer + this->m_DenseSize) bob::entity_handle(handle);
				new (this->m_ComponentBuffer + this->m_DenseSize) T(std::forward<Arg>(args)...);

				this->m_DenseSize++;
			}

			void remove(const entity_handle handle) noexcept
			{
				const uint32_t entity_dense_index = this->m_SparseBuffer[handle.index()];

				if (entity_dense_index == bob::invalid_index)
					return;

				const uint32_t last_dense_index = this->m_DenseSize - 1;

				assert(
						this->m_HandleBuffer[entity_dense_index] == handle &&
						"BOB [sparse_set][remove()]: requested for deletion of same index of different generation"
						);

				const bob::entity_handle moving_entity = this->m_HandleBuffer[last_dense_index];

				if (entity_dense_index != last_dense_index)
				{
					std::swap(this->m_ComponentBuffer[entity_dense_index], this->m_ComponentBuffer[last_dense_index]);
					std::swap(this->m_HandleBuffer[entity_dense_index], this->m_HandleBuffer[last_dense_index]);
				}

				if constexpr(!std::is_trivially_destructible_v<T>)
					this->m_ComponentBuffer[last_dense_index].~T();

				this->m_SparseBuffer[moving_entity.index()] = entity_dense_index;
				this->m_SparseBuffer[handle.index()] = bob::invalid_index;

				this->m_DenseSize--;
			}

		private:
			void m_ExtendSparseBuffer(const size_t new_size) noexcept
			{
				assert(
						new_size > this->m_SparseSize &&
						"BOB [sparse_set][m_ExtendSparse()]: new_size cannot be smaller than current sparse size"
						);

				uint32_t* new_sparse_buffer = this->m_IndexAllocator.allocate(new_size);
				std::memcpy(new_sparse_buffer, this->m_SparseBuffer, this->m_SparseSize * sizeof(uint32_t));
				this->m_IndexAllocator.deallocate(this->m_SparseBuffer, this->m_SparseSize);

				for (size_t i = this->m_SparseSize; i < new_size; i++)
					new_sparse_buffer[i] = bob::invalid_index;

				this->m_SparseBuffer = new_sparse_buffer;
				this->m_SparseSize = new_size;
			}

			void m_ExtendHandleBuffer(const size_t new_capacity) noexcept
			{
				assert(
						new_capacity > this->m_DenseCapacity &&
						"BOB [sparse_set][m_ExtendHandleBuffer()]: new_capacity must be larger than current capacity"
						);

				bob::entity_handle* new_handle_buffer = this->m_HandleAllocator.allocate(new_capacity);
				std::memcpy(new_handle_buffer, this->m_HandleBuffer, this->m_DenseSize * sizeof(bob::entity_handle));
				this->m_HandleAllocator.deallocate(this->m_HandleBuffer, this->m_DenseCapacity);
				this->m_HandleBuffer = new_handle_buffer;
			}

			void m_ExtendComponentBuffer(const size_t new_capacity) noexcept
			{
				assert(
						new_capacity > this->m_DenseCapacity &&
						"BOB [sparse_set][m_ExtendComponentBuffer()]: new_capacity must be larger than current capacity"
						);

				T* new_component_buffer = this->m_ComponentAllocator.allocate(new_capacity);

				if constexpr (!std::is_trivially_copyable_v<T>)
				{
					for (size_t i = 0, n = this->m_DenseSize; i < n; i++)
						new (new_component_buffer + i) T(std::move(this->m_ComponentBuffer[i]));

					for (size_t i = 0, n = this->m_DenseSize; i < n; i++)
						this->m_ComponentBuffer[i].~T();
				}
				else
					std::memcpy(new_component_buffer, this->m_ComponentBuffer, this->m_DenseSize * sizeof(T));

				this->m_ComponentAllocator.deallocate(this->m_ComponentBuffer, this->m_DenseCapacity);
				this->m_ComponentBuffer = new_component_buffer;
			}

			// sparse size and capacity will always be the same
			uint32_t* m_SparseBuffer;
			bob::entity_handle* m_HandleBuffer;
			T* m_ComponentBuffer;

			uint32_t m_SparseSize;
			uint32_t m_DenseSize;
			uint32_t m_DenseCapacity;

			std::allocator<uint32_t> m_IndexAllocator;
			std::allocator<bob::entity_handle> m_HandleAllocator;
			std::allocator<T> m_ComponentAllocator;
	};
};
#endif

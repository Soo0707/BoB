#ifndef BOB_SPARSE_SET
#define BOB_SPARSE_SET

#include <cstddef>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <utility>
#include <algorithm>

#include "entity_handle.hpp"

namespace bob
{
	struct abstract_sparse_set
	{
		abstract_sparse_set() = default;
		virtual ~abstract_sparse_set() = default;
	};

	template <typename T>
	class sparse_set : public abstract_sparse_set
	{
		public:
			sparse_set() = default;

			~sparse_set() override = default;

			sparse_set(const sparse_set&) = delete;
			sparse_set(sparse_set&& other) noexcept
				: m_SparseBuffer(std::move(other.m_SparseBuffer)),
				m_HandleBuffer(std::move(other.m_HandleBuffer)),
				m_ComponentBuffer(std::move(other.m_ComponentBuffer))
			{}

			sparse_set& operator=(const sparse_set&) = delete;
			sparse_set& operator=(sparse_set&& other) noexcept
			{
				if (this != &other)
				{
					this->m_SparseBuffer = std::move(other.m_SparseBuffer);
					this->m_HandleBuffer = std::move(other.m_HandleBuffer);
					this->m_ComponentBuffer = std::move(other.m_ComponentBuffer);
				}

				return *this;
			}

			const T& operator[] (const entity_handle handle) const noexcept
			{
				assert(
						handle != invalid_handle &&
						"BOB [sparse_set][operator[]]: handle is invalid"
						);

				const uint32_t dense_index = handle.index();

				assert(
						dense_index < this->m_SparseBuffer.size() &&
						"BOB [sparse_set][operator[]]: handle index is larger than sparse buffer"
						);

				assert(
						handle == this->m_HandleBuffer[dense_index] &&
						"BOB [sparse_set][operator[]]: handle resolved to entity of different generations"
						);

				return *this->m_ComponentBuffer[dense_index];
			}

			T& operator[] (const entity_handle handle) noexcept
			{
				return const_cast<T&>(std::as_const(*this)[handle]);
			}

			const std::vector<entity_handle>& get_handles() const noexcept
			{
				return this->m_HandleBuffer;
			}

			const std::vector<T>& get_raw_components() const noexcept
			{
				return this->m_ComponentBuffer;
			}

			std::vector<T>& get_raw_components() noexcept
			{
				return this->m_ComponentBuffer;
			}

			template <typename... Arg>
			void add(const entity_handle handle, Arg&&... args) noexcept
			{
				if (handle.index() >= this->m_SparseBuffer.size())
					this->m_SparseBuffer.resize(handle.index() + 1, invalid_index);
				
				this->m_SparseBuffer[handle.index()] = this->m_HandleBuffer.size();
				this->m_HandleBuffer.emplace_back(handle);
				this->m_ComponentBuffer.emplace_back(std::forward<Arg>(args)...);
			}

			void remove(const entity_handle handle) noexcept
			{
				const uint32_t entity_dense_index = this->m_SparseBuffer[handle.index()];

				if (entity_dense_index == bob::invalid_index)
					return;

				assert(
						this->m_HandleBuffer[entity_dense_index] == handle &&
						"BOB [sparse_set][remove()]: requested for deletion of same index of different generation"
						);

				const bob::entity_handle moving_entity = this->m_HandleBuffer.back();
				const uint32_t last_dense_index = this->m_HandleBuffer.size() - 1;

				if (entity_dense_index != last_dense_index)
				{
					std::swap(this->m_ComponentBuffer[last_dense_index], this->m_ComponentBuffer[entity_dense_index]);
					std::swap(this->m_HandleBuffer[last_dense_index], this->m_HandleBuffer[entity_dense_index]);
				}

				this->m_SparseBuffer[moving_entity.index()] = entity_dense_index;
				this->m_SparseBuffer[handle.index()] = invalid_index;
			}

		private:
			std::vector<uint32_t> m_SparseBuffer;
			std::vector<entity_handle> m_HandleBuffer;
			std::vector<T> m_ComponentBuffer;
	};
};
#endif

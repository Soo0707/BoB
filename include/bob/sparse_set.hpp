/*
 * Copyright (c) 2026 Soo0707
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * Use of this software is governed by the zlib license. 
 * See the LICENSE file in the project root for full license text.
 */

#ifndef BOB_SPARSE_SET
#define BOB_SPARSE_SET

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <utility>

#include "entity_handle.hpp"

namespace bob
{
	class abstract_sparse_set
	{
		public:
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

			bool has(const entity_handle handle) const noexcept
			{
				if (handle == invalid_handle)
					return false;
				
				if (handle.index() > static_cast<uint32_t>(this->m_SparseBuffer.size()))
					return false;

				const uint32_t dense_index = this->m_SparseBuffer[handle.index()];

				if (dense_index > this->m_HandleBuffer.size())
					return false;

				const entity_handle stored_handle = this->m_HandleBuffer[dense_index];

				if (handle != stored_handle)
					return false;

				return true;
			}

			const T* try_get(const entity_handle handle) const noexcept
			{
				if (this->has(handle))
					return this[handle];

				return nullptr;
			}

			T* try_get(const entity_handle handle) noexcept
			{
				return const_cast<T*>(std::as_const(*this).try_get(handle));
			}

			const T& operator[](const entity_handle handle) const noexcept
			{
				assert(
						handle != invalid_handle &&
						"BOB [sparse_set][operator[]]: handle is invalid"
						);

				assert (
						handle.index() < this->m_SparseBuffer.size() &&
						"BOB [sparse_set][operator[]]: received handle which indexed out of bounds into sparse buffer"
						);

				// i spent hours just to finally realise that i did handle.index()
				// instead of translating the external index to internal
				const uint32_t dense_index = this->m_SparseBuffer[handle.index()];

				assert(
						dense_index < this->m_HandleBuffer.size() &&
						"BOB [sparse_set][operator[]]: handle index is larger than dense buffer"
						);

				assert(
						handle == this->m_HandleBuffer[dense_index] &&
						"BOB [sparse_set][operator[]]: handle resolved to entity of different generation or invalid index"
						);

				return this->m_ComponentBuffer[dense_index];
			}

			T& operator[](const entity_handle handle) noexcept
			{
				return const_cast<T&>(std::as_const(*this)[handle]);
			}

			const std::vector<T>& get_components() const noexcept
			{
				return this->m_ComponentBuffer;
			}

			std::vector<T>& get_components() noexcept
			{
				return this->m_ComponentBuffer;
			}
			
			const std::vector<entity_handle>& get_handles() const noexcept
			{
				return this->m_HandleBuffer;
			}

			// NOTE: do NOT call add and remove directly. it is best to go through the registry.
			template <typename... Arg>
			void add(const entity_handle handle, Arg&&... args) noexcept
			{
				if (handle.index() >= this->m_SparseBuffer.size())
					this->m_SparseBuffer.resize(handle.index() + 1, invalid_index);

				this->m_SparseBuffer[handle.index()] = static_cast<uint32_t>(this->m_HandleBuffer.size());
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
				const uint32_t last_dense_index = static_cast<uint32_t>(this->m_HandleBuffer.size()) - 1;

				if (entity_dense_index != last_dense_index)
				{
					std::swap(this->m_HandleBuffer[last_dense_index], this->m_HandleBuffer[entity_dense_index]);
					std::swap(this->m_ComponentBuffer[last_dense_index], this->m_ComponentBuffer[entity_dense_index]);
				}

				this->m_SparseBuffer[moving_entity.index()] = entity_dense_index;
				this->m_SparseBuffer[handle.index()] = invalid_index;

				this->m_HandleBuffer.pop_back();
				this->m_ComponentBuffer.pop_back();
			}

			// TODO: improve this reserve function. this doesn't seem to meet real world needs.
			void reserve(const size_t new_size) noexcept
			{
				this->m_SparseBuffer.reserve(new_size);
				this->m_HandleBuffer.reserve(new_size);
				this->m_ComponentBuffer.reserve(new_size);
			}

		private:
			std::vector<uint32_t> m_SparseBuffer;
			std::vector<entity_handle> m_HandleBuffer;
			std::vector<T> m_ComponentBuffer;
	};
};
#endif

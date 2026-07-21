/*
 * Copyright (c) 2026 Soo0707
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * Use of this software is governed by the zlib license. 
 * See the LICENSE file in the project root for full license text.
 */

#ifndef BOB_ENTITY_HANDLE_GENERATOR
#define BOB_ENTITY_HANDLE_GENERATOR

#include <cassert>
#include <cstdint>
#include <vector>

#include "entity_handle.hpp"

namespace bob
{
	class entity_handle_generator
	{
		public:
			entity_handle_generator() : m_NextIndex(0)
			{}

			entity_handle get_new_handle() noexcept
			{
				entity_handle new_handle = invalid_handle;

				if (this->m_InvalidatedHandles.size() != 0)
				{
					new_handle = this->m_InvalidatedHandles.back();
					this->m_InvalidatedHandles.pop_back();

					new_handle = m_IncrementGeneration(new_handle);
				}
				else
				{
					new_handle = this->m_NextIndex;
					this->m_NextIndex++;

					assert(
							this->m_NextIndex <= 1048575 &&
							"BOB [entity_handle_generator][get_new_handle()]: ran out of indicies"
							);
				}

				return new_handle;
			}

			void invalidate_handle(const entity_handle handle) noexcept
			{
				// NOTE: this function accepts duplicates which is unavoidable without a linear scan
				assert(
						handle.index() < this->m_NextIndex &&
						"BOB [entity_handle_generator][invalidate_handle()]: invalidated handle that was never handed out"
						);
				this->m_InvalidatedHandles.emplace_back(handle);
			}

			entity_handle peak_next_handle() const noexcept
			{
				entity_handle new_handle = invalid_handle;

				if (this->m_InvalidatedHandles.size() != 0)
				{
					new_handle = this->m_InvalidatedHandles.back();
					new_handle = this->m_IncrementGeneration(new_handle);
				}
				else
				{
					const uint32_t next_index = this->m_NextIndex;
					assert(
							this->m_NextIndex <= 1048575 &&
							"BOB [entity_handle_generator][peak_next_handle()]: ran out of indicies"
							);
					new_handle = entity_handle(next_index);
				}

				return new_handle;
			}

		private:
			entity_handle m_IncrementGeneration(const entity_handle handle) const noexcept
			{
				uint32_t generation = handle.generation();

				if (generation + 1 == 4096)
					generation = 0;
				else
					generation += 1;

				const uint32_t new_value = (generation << 20) | handle.index();
				return entity_handle(new_value);
			}

			std::vector<entity_handle> m_InvalidatedHandles;
			uint32_t m_NextIndex;
	};
};
#endif

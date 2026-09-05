/*
 * Copyright (c) 2026 Soo0707
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * Use of this software is governed by the zlib license. 
 * See the LICENSE file in the project root for full license text.
 */

#ifndef BOB_HANDLE_GENERATOR
#define BOB_HANDLE_GENERATOR

#include <cassert>
#include <cstdint>
#include <vector>

#include "bob/entity.hpp"

namespace bob
{
	class handle_generator
	{
		public:
			handle_generator() : m_NextIndex(0)
			{}

			entity create_handle() noexcept
			{
				entity new_handle = invalid_handle;

				if (m_InvalidatedHandles.size() != 0)
				{
					new_handle = m_InvalidatedHandles.back();
					m_InvalidatedHandles.pop_back();

					new_handle = m_IncrementGeneration(new_handle);
				}
				else
				{
					new_handle = m_NextIndex;
					m_NextIndex++;

					assert(
							m_NextIndex <= 1048575 &&
							"BOB [handle_generator][get_new_handle()]: ran out of indicies"
							);
				}

				return new_handle;
			}

			void invalidate_handle(const entity handle) noexcept
			{
				// NOTE: this function accepts duplicates which is unavoidable without a linear scan
				assert(
						handle.index() < m_NextIndex &&
						"BOB [handle_generator][invalidate_handle()]: invalidated handle that was never handed out"
						);
				m_InvalidatedHandles.emplace_back(handle);
			}

		private:
			entity m_IncrementGeneration(const entity handle) const noexcept
			{
				uint32_t generation = handle.generation();

				if (generation + 1 == 4096)
					generation = 0;
				else
					generation += 1;

				const uint32_t new_value = (generation << 20) | handle.index();
				return entity(new_value);
			}

			std::vector<entity> m_InvalidatedHandles;
			uint32_t m_NextIndex;
	};
};
#endif

/*
 * Copyright (c) 2026 Soo0707
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * Use of this software is governed by the zlib license. 
 * See the LICENSE file in the project root for full license text.
 */

#ifndef BOB_ENTITY
#define BOB_ENTITY

#include <cstdint>

namespace bob
{
	class entity
	{
		public:
			entity() = delete;

			constexpr entity(const uint32_t handle):
				m_Handle(handle)
			{}

			constexpr uint32_t generation() const noexcept
			{
				return (m_Handle >> 20) & 0xfff;
			}

			constexpr uint32_t index() const noexcept
			{
				return m_Handle & 0x000fffff;
			}

			constexpr uint32_t value() const noexcept
			{
				return m_Handle;
			}

			constexpr bool operator==(const entity other) const noexcept
			{
				return m_Handle == other.m_Handle;
			}

			constexpr bool operator!=(const entity other) const noexcept
			{
				return m_Handle != other.m_Handle;
			}

		private:
			uint32_t m_Handle;
	};

	inline constexpr entity invalid_handle { 0xffffffff };
	inline constexpr uint32_t invalid_index { 0xffffffff };
	inline constexpr uint32_t max_index { 0xffffffff };
};
#endif

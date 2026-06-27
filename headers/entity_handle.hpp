#ifndef BOB_ENTITY_HANDLE
#define BOB_ENTITY_HANDLE

#include <cstdint>

namespace bob
{
	class entity_handle
	{
		public:
			entity_handle() = delete;

			constexpr entity_handle(const uint32_t handle):
				m_Handle(handle)
			{}

			[[nodiscard]] constexpr uint32_t generation() const noexcept
			{
				return (this->m_Handle >> 20) & 0xfff;
			}

			[[nodiscard]] constexpr uint32_t index() const noexcept
			{
				return this->m_Handle & 0x000fffff;
			}

			[[nodiscard]] constexpr uint32_t value() const noexcept
			{
				return this->m_Handle;
			}

			[[nodiscard]] constexpr bool operator==(const entity_handle other) const noexcept
			{
				return this->m_Handle == other.m_Handle;
			}

			[[nodiscard]] constexpr bool operator!=(const entity_handle other) const noexcept
			{
				return this->m_Handle != other.m_Handle;
			}

		private:
			uint32_t m_Handle;
	};

	inline constexpr entity_handle invalid_handle = entity_handle(0xffffffff);
	inline constexpr uint32_t invalid_index { 0xffffffff };
	inline constexpr uint32_t max_index { 0xffffffff };
};
#endif

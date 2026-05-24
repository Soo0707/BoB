#ifndef BOB_COMPONENT_HANDLE
#define BOB_COMPONENT_HANDLE

#include <cstdint>

namespace bob
{
	struct component_handle_counter
	{
		static inline uint8_t s_NextIndex = 0;
	};

	template <typename T>
	inline uint8_t component_handle() noexcept
	{
		static const uint8_t index = component_handle_counter::s_NextIndex++;
		return index;
	}
};
#endif

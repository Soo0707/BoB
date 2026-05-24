#ifndef BOB_ENTITY_HANDLE_GENERATOR
#define BOB_ENTITY_HANDLE_GENERATOR

#include <cstdint>
#include <vector>

#include "entity_handle.hpp"

namespace bob
{
	class entity_handle_generator
	{
		public:
			entity_handle_generator();

			entity_handle get_new_handle() noexcept;
			void invalidate_handle(const entity_handle handle) noexcept;

		private:
			entity_handle increment_generation(const entity_handle handle) const noexcept;

			std::vector<entity_handle> m_InvalidatedHandles;
			static inline uint32_t s_NextIndex = 0;
	}
};
#endif

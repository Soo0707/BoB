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

			uint32_t get_handle() noexcept;
			void mark_dead() noexcept;

		private:
			std::vector<uint32_t> m_DeadIDs;
			static inline uint32_t m_NextID = 0;
	}
};
#endif

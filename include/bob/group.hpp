#ifndef BOB_GROUP
#define BOB_GROUP

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

#include "entity_handle.hpp"
#include "sparse_set.hpp"

namespace bob
{
	class abstract_group
	{};

	template <typename Component>
	class group_field
	{
		public:
			sparse_set<Component>* data;
	};

	template <typename... Components>
	class group :
		public abstract_group,
		public group_field<Components>...
	{
		public:
			group(sparse_set<Components>*... ptr) :
				m_Size(0)
			{
				(this->m_SetContainerPtr<Components>(ptr), ...);
			}

			template <typename T>
			std::vector<T>& container() noexcept
			{
				return *(static_cast<group_field<T>*>(this)->data);
			}

			void add_callback(const entity_handle handle) noexcept
			{
				const bool valid = (this->container<Components>().has(handle) && ...);

				if (valid)
				{
					// TODO: swap for all sets to the correct position
					this->m_Size++;
				}
			}

			void remove_callback(const entity_handle handle) noexcept
			{
				this->m_Size--;
			}

		private:
			template <typename T>
			void m_SetContainerPtr(std::vector<T>* ptr)
			{
				static_cast<group_field<T>*>(this)->data = ptr;
			}

			size_t m_Size;
	};
};

#endif

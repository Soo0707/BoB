#ifndef BOB_GROUP
#define BOB_GROUP

#include <cstddef>
#include <vector>

#include "bob/entity_handle.hpp"
#include "bob/sparse_set.hpp"
#include "bob/utilities.hpp"

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

			add_proxy add_callback() const noexcept
			{
				return add_proxy(this, &execute<decltype(this), &group<Components...>::m_AddCallbackImpl>);
			}

			remove_proxy remove_callback() const noexcept
			{
				return remove_proxy(this, &execute<decltype(this), &group<Components...>::m_RemoveCallbackImpl>);
			}

		private:
			template <typename T>
			void m_SetContainerPtr(std::vector<T>* ptr)
			{
				static_cast<group_field<T>*>(this)->data = ptr;
			}

			void m_AddCallbackImpl(const entity_handle handle) noexcept
			{
				const bool valid = (this->container<Components>().has(handle) && ...);

				if (valid)
				{
					// TODO: swap for all sets to the correct position
					this->m_Size++;
				}
			}

			void m_RemoveCallbackImpl() noexcept
			{
				this->m_Size--;
			}

			size_t m_Size;
	};
};

#endif

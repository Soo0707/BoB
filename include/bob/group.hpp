#ifndef BOB_GROUP
#define BOB_GROUP

#include <cstddef>

#include "bob/entity_handle.hpp"
#include "bob/sparse_set.hpp"
#include "bob/utilities.hpp"

namespace bob
{
	class abstract_group
	{
		public:
			virtual ~abstract_group() = default;
	};

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
			sparse_set<T>& container() noexcept
			{
				return *(static_cast<group_field<T>*>(this)->data);
			}

			proxy add_callback() noexcept
			{
				return proxy(this, &execute<group<Components...>, &group<Components...>::m_AddCallbackImpl>);
			}

			proxy remove_callback() noexcept
			{
				return proxy(this, &execute<group<Components...>, &group<Components...>::m_RemoveCallbackImpl>);
			}

			size_t size() const noexcept
			{
				return this->m_Size;
			}

		private:
			template <typename T>
			void m_SetContainerPtr(sparse_set<T>* ptr) noexcept
			{
				static_cast<group_field<T>*>(this)->data = ptr;
			}

			void m_AddCallbackImpl(const entity_handle handle) noexcept
			{
				const bool valid = (this->container<Components>().has(handle) && ...);

				if (valid)
				{
					this->m_Size++;
					(this->container<Components>().shift(handle, this->m_Size - 1), ...);
				}
			}

			void m_RemoveCallbackImpl(const entity_handle handle) noexcept
			{
				const bool valid = (this->container<Components>().has(handle) && ...);

				if (valid)
				{
					(this->container<Components>().shift(handle, this->m_Size - 1), ...);
					this->m_Size--;
				}
			}

			size_t m_Size;
	};
};

#endif

#ifndef BOB_REGISTRY
#define BOB_REGISTRY

#include <vector>
#include <cstddef>
#include <algorithm>
#include <memory>

#include "sparse_set.hpp"
#include "component_handle.hpp"
#include "entity_handle.hpp"
#include "entity_handle_generator.hpp"

namespace bob
{
	class registry
	{
		public:
			registry();

			template <typename T>
			void register_component() noexcept
			{
				const size_t component_index = component_handle<T>();

				this->m_Sets.resize(std::max(this->m_Sets, component_index + 1));

				if (this->m_Sets[component_index] != nullptr)
					return;

				this->m_Sets[component_index] = std::make_unique<sparse_set<T>>();
			}

			template <typename T, typename... Arg>
			void add(Arg&&... args) noexcept
			{
			}

			template <typename T>
			void remove(const entity_handle handle) noexcept
			{
				const size_t component_index = component_handle<T>();

				if (component_index > this->m_Sets.size())
					return;

				abstract_sparse_set* base_ptr = this->m_Sets[component_index].get();
				sparse_set<T>* child_ptr = static_cast<sparse_set<T>*>(base_ptr);

				child_ptr->remove(handle);
			}

		private:
			std::vector<std::unique_ptr<abstract_sparse_set>> m_Sets;
	};
};
#endif

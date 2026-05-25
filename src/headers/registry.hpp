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
			registry() = default;

			entity_handle get_new_handle() noexcept
			{
				return m_HandleGenerator.get_new_handle();
			}

			void invalidate_handle(const entity_handle handle) noexcept
			{
				m_HandleGenerator.invalidate_handle(handle);
			}

			template <typename T>
			void register_component() noexcept
			{
				const size_t component_index = component_handle<T>();

				this->m_Sets.resize(std::max(this->m_Sets, component_index + 1));

				assert(this->m_Sets[component_index] == nullptr && "BOB [registry][register_component()]: a component has been registered twice.");

				this->m_Sets[component_index] = std::make_unique<sparse_set<T>>();
			}

			template <typename T, typename... Arg>
			void add(const entity_handle handle, Arg&&... args) noexcept
			{
				// TODO: reallocate sparse if next handle has larger indicies
				entity_handle peak_next_handle() const noexcept

				const size_t component_index = component_handle<T>();

				assert(
						component_index < this->m_Sets.size() &&
						"BOB [registry][add()]: component_index is larger than number of sparse sets. did you forget to register a component?"
						);

				assert(
						this->m_Sets[component_index] != nullptr &&
						"BOB [registry][add()]: component_index indexed to a nullptr. did you forget to register a component?"
						);

				abstract_sparse_set* base_ptr = this->m_Sets[component_index].get();
				sparse_set<T>* child_ptr = static_cast<sparse_set<T>*>(base_ptr);

				entity_handle new_handle = this->m_HandleGenerator.get_new_handle();
				child_ptr->add(handle, std::forward<Arg>(args)...);
			}

			template <typename T>
			void remove(const entity_handle handle) noexcept
			{
				const size_t component_index = component_handle<T>();

				if (component_index >= this->m_Sets.size())
					return;

				abstract_sparse_set* base_ptr = this->m_Sets[component_index].get();
				sparse_set<T>* child_ptr = static_cast<sparse_set<T>*>(base_ptr);

				child_ptr->remove(handle);
			}

		private:
			std::vector<std::unique_ptr<abstract_sparse_set>> m_Sets;
			entity_handle_generator m_HandleGenerator;
	};
};
#endif

#ifndef BOB_REGISTRY
#define BOB_REGISTRY

#include <vector>
#include <cstddef>
#include <cassert>
#include <algorithm>
#include <memory>

#include "sparse_set.hpp"
#include "component_handle.hpp"
#include "entity_handle.hpp"
#include "entity_handle_generator.hpp"

namespace bob
{
	template <typename... Component>
	class registry
	{
		public:
			registry() : m_CurrentSparseSize(16)
			{
				static_assert(sizeof...(T) > 0 && "BOB [registry][registery()]: no component types were passed in for registration");

				(this->m_RegisterComponent<Component>(), ...);

				this->m_ExtendAllSparse();
			}

			entity_handle get_new_handle() noexcept
			{
				return m_HandleGenerator.get_new_handle();
			}

			template <typename T, typename... Arg>
			void add(const entity_handle handle, Arg&&... args) noexcept
			{
				const entity_handle next_handle = this->m_HandleGenerator.peak_next_handle();

				if (next_handle.index() > this->m_CurrentSparseSize)
					this->m_ExtendAllSparse();

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

			template <typename... T>
			void remove(const entity_handle handle) noexcept
			{
				static_assert(sizeof...(T) > 0 && "BOB [registry][remove()]: number of components to be removed must be more than 0");
				(this->m_RemoveComponent<T>(handle), ...);

				this->m_HandleGenerator.invalidate_handle(handle);
			}

		private:
			template <typename T>
			void m_RegisterComponent() noexcept
			{
				const size_t component_index = component_handle<T>();

				this->m_Sets.resize(std::max(this->m_Sets.size(), component_index + 1));

				assert(this->m_Sets[component_index] == nullptr && "BOB [registry][m_RegisterComponent()]: a component has been registered twice.");

				this->m_Sets[component_index] = std::make_unique<sparse_set<T>>();
			}

			template <typename T>
			void m_RemoveComponent(const entity_handle handle) noexcept
			{
				constexpr size_t component_index = component_handle<T>();

				assert(
						component_index < this->m_Sets.size() &&
						"BOB [registry][m_RemoveComponent()]: component_index is larger than register set size. did you forget to register a component?"
						);

				abstract_sparse_set* base_ptr = this->m_Sets[component_index].get();
				sparse_set<T>* child_ptr = static_cast<sparse_set<T>*>(base_ptr);

				child_ptr->remove(handle);
			}

			template <typename T>
			void m_ExtendSparse(const size_t new_size) noexcept
			{
				constexpr size_t component_index = component_handle<T>();

				assert(
						component_index < this->m_Sets.size() &&
						"BOB [registry][m_ExtendSparse()]: component_index is larger than register set size, did you forget to register a component?"
						);

				abstract_sparse_set* base_ptr = this->m_Sets[component_index].get();
				sparse_set<T>* child_ptr = static_cast<sparse_set<T>*>(base_ptr);

				child_ptr->extend_sparse(new_size);
			}

			void m_ExtendAllSparse() noexcept
			{
				this->m_CurrentSparseSize *= 2;

				(this->m_ExtendSparse<Components>(this->m_CurrentSparseSize), ...);
			}

			std::vector<std::unique_ptr<abstract_sparse_set>> m_Sets;
			entity_handle_generator m_HandleGenerator;
			size_t m_CurrentSparseSize;
	};
};
#endif

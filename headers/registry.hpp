#ifndef BOB_REGISTRY
#define BOB_REGISTRY

#include <vector>
#include <cstddef>
#include <cassert>
#include <algorithm>
#include <memory>
#include <tuple>
#include <utility>

#include "sparse_set.hpp"
#include "entity_handle.hpp"
#include "entity_handle_generator.hpp"

namespace bob
{
	template <typename... Component>
	class registry
	{
		public:
			using component_types = std::tuple<Component...>;

			registry()
			{
				static_assert(
						sizeof...(Component) > 0 &&
						"BOB [registry][registry()]: no component types were passed in for registration"
						);

				(m_RegisterComponent<Component>(), ...);
			}

			entity_handle get_new_handle() noexcept
			{
				return m_HandleGenerator.get_new_handle();
			}

			template <typename T, typename... Arg>
			void add(const entity_handle handle, Arg&&... args) noexcept
			{
				sparse_set<T>& concrete_set = m_DowncastSparse<T>();
				concrete_set.add(handle, std::forward<Arg>(args)...);
			}

			template <typename... T>
			void remove(const entity_handle handle) noexcept
			{
				static_assert(
						sizeof...(T) > 0 &&
						"BOB [registry][remove()]: number of components to be removed must be more than 0"
						);

				(m_RemoveComponent<T>(handle), ...);
				this->m_HandleGenerator.invalidate_handle(handle);
			}
			
			template <typename First, typename... After>
			const std::vector<entity_handle>& get_handles() const noexcept
			{
				/*
				TODO: it might be a good idea to sort the arbitrary ordered components at compile time
				but that is way above my paygrade. this might still be faster than manual iteration and a
				virtual query but not sorting means that we're accessing m_Sets in an effectively arbitrary order
				which would likely cause cache misses.

				branch predictor go brrrrrrrrrrrrrrr

				note to self: this stupid template costed you hours of your sanity
				a const reference is non reassignable, to do this correctly, you need to make a pointer
				 */
				const std::vector<entity_handle>* smallest = &m_GetHandleLayer<First>();

				if constexpr (sizeof...(After) > 0)
					((smallest = &m_SelectSmallerSparse<After>(*smallest)), ...);

				return *smallest;
			}
			// TODO: get sparse function so that we can use handles to translate into dense layer
			template <typename T>
			std::vector<T>& get_component() noexcept
			{
				sparse_set<T>& concrete = m_DowncastSparse<T>();
				return concrete.get_components();
			}

		private:
			template <typename T, size_t Index = 0>
			constexpr size_t m_ComponentHandle() const noexcept
			{
				static_assert(
						Index < std::tuple_size_v<component_types> &&
						"BOB [registry][m_ComponentHandle()]: could not resolve component handle to index. did you forget to register a component?"
						);

				if constexpr (std::is_same_v<T, std::tuple_element_t<Index, component_types>>)
					return Index;
				else
					return m_ComponentHandle<T, Index + 1>();
			}

			template <typename T>
			void m_RegisterComponent() noexcept
			{
				const size_t component_index = m_ComponentHandle<T>();
				this->m_Sets.resize(std::max(this->m_Sets.size(), component_index + 1));

				assert(
						this->m_Sets[component_index] == nullptr &&
						"BOB [registry][m_RegisterComponent()]: a component has been registered twice."
						);
				this->m_Sets[component_index] = std::make_unique<sparse_set<T>>();
			}
			
			template <typename T>
			const sparse_set<T>& m_DowncastSparse() const noexcept
			{
				constexpr size_t component_index = m_ComponentHandle<T>();

				assert(
						component_index < this->m_Sets.size() &&
						"BOB [registry][m_DowncastSparse()]: component_index less than register internal set size. did you forget to register a component?"
						);

				abstract_sparse_set* base_ptr = this->m_Sets[component_index].get();
				const sparse_set<T>* child_ptr = static_cast<sparse_set<T>*>(base_ptr);

				return *child_ptr;
			}

			template <typename T>
			sparse_set<T>& m_DowncastSparse() noexcept
			{
				return const_cast<sparse_set<T>&>(std::as_const(*this).template m_DowncastSparse<T>());
			}

			template <typename T>
			const std::vector<entity_handle>& m_GetHandleLayer() const noexcept
			{
				const sparse_set<T>& concrete_set = m_DowncastSparse<T>();
				return concrete_set.get_handles();
			}

			template <typename T>
			std::vector<entity_handle>& m_GetHandleLayer() noexcept
			{
				return const_cast<sparse_set<T>&>(std::as_const(*this).template m_GetHandleLayer());
			}

			template <typename T>
			const std::vector<entity_handle>& m_SelectSmallerSparse(const std::vector<entity_handle>& current) const noexcept
			{
				const std::vector<entity_handle>& next = m_GetHandleLayer<T>();
				return (next.size() < current.size()) ? next : current;
			}

			template <typename T>
			void m_RemoveComponent(const entity_handle handle) noexcept
			{
				sparse_set<T>& concrete_set = m_DowncastSparse<T>();
				concrete_set.remove(handle);
			}

			std::vector<std::unique_ptr<abstract_sparse_set>> m_Sets;
			entity_handle_generator m_HandleGenerator;
	};
};
#endif

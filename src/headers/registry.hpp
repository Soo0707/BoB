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
			using component_types = std::tuple<Components...>;

			registry() : m_CurrentSparseSize(16)
			{
				static_assert(
						sizeof...(Component) > 0 &&
						"BOB [registry][registry()]: no component types were passed in for registration"
						);
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

				sparse_set<T>& concrete_set = this->m_DowncastSparse<T>();

				entity_handle new_handle = this->m_HandleGenerator.get_new_handle();
				concrete_set.add(handle, std::forward<Arg>(args)...);
			}

			template <typename... T>
			void remove(const entity_handle handle) noexcept
			{
				static_assert(
						sizeof...(T) > 0 &&
						"BOB [registry][remove()]: number of components to be removed must be more than 0"
						);
				(this->m_RemoveComponent<T>(handle), ...);

				this->m_HandleGenerator.invalidate_handle(handle);
			}
			
			template <typename T, size_t Index = 0>
			constexpr size_t component_handle() const noexcept
			{
				static_assert(
						Index < std::tuple_size_v<component_types> &&
						"BOB [registry][component_handle()]: could not resolve component handle to index. did you forget to register a component?"
						);

				if constexpr (std::is_same_v<T, std::tuple_element_t<Index, component_types>>)
					return Index;
				else
					return component_handle<T, Index + 1>();
			}

			template <typename First, typename... After>
			handle_proxy get_handles() noexcept
			{
				/*
				TODO: it might be a good idea to sort the arbitrary ordered components at compile time
				but that is way above my paygrade. this might still be faster than manual iteration and a
				virtual query but not sorting means that we're accessing m_Sets in an effectively arbitrary order
				which would likely cause cache misses.

				branch predictor go brrrrrrrrrrrrrrr
				 */
				handle_proxy smallest = this->m_GetHandleLayer<First>();

				if constexpr (sizeof...(After) > 0)
					((smallest = this->m_SelectSmallerSparse<After>(smallest)), ...);

				return smallest;
			}

			template <typename T>
			component_proxy<T> get_component() noexcept
			{
				sparse_set<T>* concrete = this->m_DowncastSparse<T>();
				return concrete.
			}

		private:
			template <typename T>
			void m_RegisterComponent() noexcept
			{
				const size_t component_index = component_handle<T>();
				this->m_Sets.resize(std::max(this->m_Sets.size(), component_index + 1));

				assert(
						this->m_Sets[component_index] == nullptr &&
						"BOB [registry][m_RegisterComponent()]: a component has been registered twice."
						);
				this->m_Sets[component_index] = std::make_unique<sparse_set<T>>();
			}

			template <typename T>
			#if defined(_MSC_VER) __forceinline #elif defined(__GNUC__) || defined(__clang__) __attribute__((always_inline)) #endif 
			sparse_set<T>& m_DowncastSparse() noexcept
			{
				constexpr size_t component_index = component_handle<T>();

				assert(
						component_index < this->m_Sets.size() &&
						"BOB [registry][m_DowncastSparse()]: component_index less than register internal set size. did you forget to register a component?"
						);

				abstract_sparse_set* base_ptr = this->m_Sets[component_index].get();
				sparse_set<T>* child_ptr = static_cast<sparse_set<T>*>(base_ptr);

				return *child_ptr;
			}

			template <typename T>
			void m_RemoveComponent(const entity_handle handle) noexcept
			{
				sparse_set<T>& concrete_set = this->m_DowncastSparse<T>();
				concrete_set.remove(handle);
			}

			template <typename T>
			void m_ExtendSparse(const size_t new_size) noexcept
			{
				sparse_set<T>& concrete_set = this->m_DowncastSparse<T>();
				concrete_set.extend_sparse(new_size);
			}

			template <typename T>
			handle_proxy m_GetHandleLayer() const noexcept
			{
				sparse_set<T>& concrete_set = this->m_DowncastSparse<T>();
				return concrete_set.get_handles();
			}

			template <typename T>
			handle_proxy m_SelectSmallerSparse(const handle_proxy current) const noexcept
			{
				const handle_proxy next = this->m_GetHandleLayer<T>();
				return (next.size < current.size) ? next : current;
			}

			void m_ExtendAllSparse() noexcept
			{
				this->m_CurrentSparseSize *= 2;
				(this->m_ExtendSparse<Component>(this->m_CurrentSparseSize), ...);
			}

			std::vector<std::unique_ptr<abstract_sparse_set>> m_Sets;
			entity_handle_generator m_HandleGenerator;
			size_t m_CurrentSparseSize;
	};
};
#endif

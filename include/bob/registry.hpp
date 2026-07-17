#ifndef BOB_REGISTRY
#define BOB_REGISTRY

#include <cassert>
#include <cstddef>
#include <tuple>
#include <vector>

#include "entity_handle_generator.hpp"
#include "entity_handle.hpp"
#include "sparse_set.hpp"

namespace bob
{
	template <typename... Component>
	class registry
	{
		public:
			using components = std::tuple<Component...>;

			registry()
			{
				static_assert(
						sizeof...(Component) > 0 &&
						"BOB [registry][registry()]: no component types were passed in for registration"
						);
			}

			entity_handle get_new_handle() noexcept
			{
				return this->m_HandleGenerator.get_new_handle();
			}

			template <typename T, typename... Arg>
			void add(const entity_handle handle, Arg&&... args) noexcept
			{
				sparse_set<T>& concrete_set = std::get<sparse_set<T>>(this->m_Sets);
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
			const std::vector<entity_handle>& get_iterator() const noexcept
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
			
			template <typename T>
			const sparse_set<T>& get_sparse_set() const noexcept
			{
				return std::get<sparse_set<T>>(this->m_Sets);
			}

			template <typename T>
			sparse_set<T>& get_sparse_set() noexcept
			{
				// if i looked up the syntax i might as well use it.
				return const_cast<sparse_set<T>&>(std::as_const(*this).template get_sparse_set<T>());
			}

		private:
			template <typename T>
			const std::vector<entity_handle>& m_GetHandleLayer() const noexcept
			{
				const sparse_set<T>& concrete_set = std::get<sparse_set<T>>(this->m_Sets);
				return concrete_set.get_handles();
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
				sparse_set<T>& concrete_set = std::get<sparse_set<T>>(this->m_Sets);
				concrete_set.remove(handle);
			}

			std::tuple<sparse_set<Component>...> m_Sets;
			entity_handle_generator m_HandleGenerator;
	};
};
#endif

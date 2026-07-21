/*
 * Copyright (c) 2026 Soo0707
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * Use of this software is governed by the zlib license. 
 * See the LICENSE file in the project root for full license text.
 */

#ifndef BOB_REGISTRY
#define BOB_REGISTRY

#include <cassert>
#include <cstddef>
#include <memory>
#include <vector>

#include "entity_handle_generator.hpp"
#include "entity_handle.hpp"
#include "sparse_set.hpp"

namespace bob
{
	class registry
	{
		public:
			registry()
			{}

			entity_handle get_new_handle() noexcept
			{
				return this->m_HandleGenerator.get_new_handle();
			}

			void release_handle(const entity_handle handle) noexcept
			{
				this->m_HandleGenerator.invalidate_handle(handle);
			}

			template <typename T>
			void register_component() noexcept
			{
				const size_t type_index = this->m_GetTypeIndex<T>();

				this->m_Sets.resize(type_index + 1);

				assert(this->m_Sets[type_index] == nullptr && "BOB [registry][register_component()]: component registered twice");

				this->m_Sets[type_index] = std::make_unique<sparse_set<T>>();
			}

			template <typename T, typename... Arg>
			void add(const entity_handle handle, Arg&&... args) noexcept
			{
				sparse_set<T>& concrete_set = this->get_sparse_set<T>();
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
				const size_t type_index = this->m_GetTypeIndex<T>();

				assert(type_index < this->m_Sets.size() && "BOB [registry][get_sparse_set]: called on unregistered component");
				return *static_cast<sparse_set<T>*>(this->m_Sets[type_index].get());
			}

			template <typename T>
			sparse_set<T>& get_sparse_set() noexcept
			{
				// if i looked up the syntax i might as well use it.
				return const_cast<sparse_set<T>&>(std::as_const(*this).get_sparse_set<T>());
			}

		private:
			template <typename T>
			size_t m_GetTypeIndex() const noexcept
			{
				static const size_t type_index = registry::m_TypeCounter++;
				return type_index;
			}

			template <typename T>
			const std::vector<entity_handle>& m_GetHandleLayer() const noexcept
			{
				const sparse_set<T>& concrete_set = this->get_sparse_set<T>();
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
				sparse_set<T>& concrete_set = this->get_sparse_set<T>();
				concrete_set.remove(handle);
			}

			std::vector<std::unique_ptr<abstract_sparse_set>> m_Sets;
			entity_handle_generator m_HandleGenerator;
			static inline size_t m_TypeCounter = 0;
	};
};
#endif

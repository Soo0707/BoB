/*
 * Copyright (c) 2026 Soo0707
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * Use of this software is governed by the zlib license. 
 * See the LICENSE file in the project root for full license text.
 */

#ifndef BOB_REGISTRY_TEST
#define BOB_REGISTRY_TEST

#include "bob/registry.hpp"

#include <string>
#include <cstddef>
#include <cassert>

struct Vector2
{
	float x;
	float y;
};

struct Tag
{};


class RegistryTest
{
	public:
		RegistryTest()
		{
			this->m_Registry.register_component<std::string>();
			this->m_Registry.register_component<Vector2>();
			this->m_Registry.register_component<Tag>();

			this->m_Reserve();

			this->m_AddEntityZero();
			this->m_AddEntityOne();
			this->m_AddEntityTwo();

			this->m_GetStringComponents();
			this->m_GetVectorComponents();
			this->m_GetTagComponents();

			this->m_IterateStrings();
			this->m_IterateStringsAndVectors();
			this->m_IterateAll();

			this->m_RemoveEntityTwo();
			this->m_RemoveEntityOne();
			this->m_RemoveEntityZero();
		}

	private:
		void m_AddEntityZero()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			const bob::entity_handle first_handle = this->m_Registry.get_new_handle();
			assert(first_handle == bob::entity_handle(0));

			this->m_Registry.add<std::string>(first_handle, "0");
			
			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}

		void m_AddEntityOne()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			const bob::entity_handle second_handle = this->m_Registry.get_new_handle();
			assert(second_handle == bob::entity_handle(1));

			this->m_Registry.add<Vector2>(second_handle, 6.0f, 7.0f);
			this->m_Registry.add<std::string>(second_handle, "1");
			
			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}

		void m_AddEntityTwo()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			const bob::entity_handle third_handle = this->m_Registry.get_new_handle();
			assert(third_handle == bob::entity_handle(2));

			this->m_Registry.add<Tag>(third_handle);
			this->m_Registry.add<Vector2>(third_handle, 12.0f, 14.0f);
			this->m_Registry.add<std::string>(third_handle, "2");
			
			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}

		void m_Reserve()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			this->m_Registry.reserve<std::string, Vector2, Tag>(1024);

			const size_t string_set_capacity = this->m_Registry.get_sparse_set<std::string>().get_components().capacity();
			const size_t vector_set_capacity = this->m_Registry.get_sparse_set<Vector2>().get_components().capacity();
			const size_t tag_set_capacity = this->m_Registry.get_sparse_set<Tag>().get_components().capacity();

			assert(string_set_capacity >= 1024);
			assert(vector_set_capacity >= 1024);
			assert(tag_set_capacity >= 1024);

			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}

		void m_GetStringComponents()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			const auto& string_set = this->m_Registry.get_sparse_set<std::string>();
			const auto& string_components = string_set.get_components();

			assert(string_components.size() == 3);
			
			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}

		void m_GetVectorComponents()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			const auto& vector_set = this->m_Registry.get_sparse_set<Vector2>();
			const auto& vector_components = vector_set.get_components();

			assert(vector_components.size() == 2);
			
			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}
		
		void m_GetTagComponents()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			const auto& tag_set = this->m_Registry.get_sparse_set<Tag>();
			const auto& tag_components = tag_set.get_components();

			assert(tag_components.size() == 1);

			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}

		void m_IterateStrings()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			const auto& string_handles = this->m_Registry.get_iterator<std::string>();
			assert(string_handles.size() == 3);

			const bob::sparse_set<std::string>& string_set = this->m_Registry.get_sparse_set<std::string>();

			for (size_t i = 0, n = string_handles.size(); i < n; ++i)
			{
				const bob::entity_handle handle = string_handles[i];

				assert(string_set[handle] == std::to_string(i));
			}

			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}

		void m_IterateStringsAndVectors()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			const auto& string_vector_handles = this->m_Registry.get_iterator<Vector2, std::string>();
			assert(string_vector_handles.size() == 2);

			const bob::sparse_set<std::string>& string_set = this->m_Registry.get_sparse_set<std::string>();
			const bob::sparse_set<Vector2>& vector_set = this->m_Registry.get_sparse_set<Vector2>();

			for (size_t i = 0, n = string_vector_handles.size(); i < n; ++i)
			{
				const bob::entity_handle handle = string_vector_handles[i];

				assert(string_set[handle] == std::to_string(i + 1));
				assert(vector_set[handle].x == static_cast<float>((i + 1) * 6.0f));
				assert(vector_set[handle].y == static_cast<float>((i + 1) * 7.0f));
			}

			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}

		void m_IterateAll()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			const auto& string_vector_tag_handles = this->m_Registry.get_iterator<Tag, Vector2, std::string>();
			assert(string_vector_tag_handles.size() == 1);

			const bob::sparse_set<std::string>& string_set = this->m_Registry.get_sparse_set<std::string>();
			const bob::sparse_set<Vector2>& vector_set = this->m_Registry.get_sparse_set<Vector2>();

			const bob::entity_handle handle = string_vector_tag_handles[0];

			assert(string_set[handle] == "2");
			assert(vector_set[handle].x == 12.0f);
			assert(vector_set[handle].y == 14.0f);

			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}

		void m_RemoveEntityTwo()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			const bob::entity_handle third_handle = bob::entity_handle(2);

			this->m_Registry.remove<Tag, Vector2, std::string>(third_handle);
			this->m_Registry.release_handle(third_handle);

			const auto& string_vector_tag_handles = this->m_Registry.get_iterator<Tag, Vector2, std::string>();
			assert(string_vector_tag_handles.size() == 0);

			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}

		void m_RemoveEntityOne()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			const bob::entity_handle second_handle = bob::entity_handle(1);

			this->m_Registry.remove<Vector2, std::string>(second_handle);
			this->m_Registry.release_handle(second_handle);

			const auto& string_vector_handles = this->m_Registry.get_iterator<Vector2, std::string>();
			assert(string_vector_handles.size() == 0);

			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}

		void m_RemoveEntityZero()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			const bob::entity_handle first_handle = bob::entity_handle(0);

			this->m_Registry.remove<std::string>(first_handle);
			this->m_Registry.release_handle(first_handle);

			const auto& string_handles = this->m_Registry.get_iterator<std::string>();
			assert(string_handles.size() == 0);

			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}

		bob::registry m_Registry;
};

#endif

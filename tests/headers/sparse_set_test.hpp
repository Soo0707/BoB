/*
 * Copyright (c) 2026 Soo0707
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * Use of this software is governed by the zlib license. 
 * See the LICENSE file in the project root for full license text.
 */

#ifndef BOB_SPARSE_SET_TEST
#define BOB_SPARSE_SET_TEST

#include "bob/sparse_set.hpp"

#include <iostream>
#include <cassert>
#include <cstddef>

struct TagB
{};

class SparseSetTest
{
	public:
		SparseSetTest()
		{
			this->m_AppendSparse();
			this->m_TestMiddle();
			this->m_TestComponentsThenRemove();
		}

	private:
		// TODO: test reserve
		
		void m_AppendSparse()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			for (size_t i = 0; i < 4; i++)
			{
				const bob::entity_handle handle = bob::entity_handle(static_cast<uint32_t>(i));
				this->m_Sparse.add(handle);
			}

			const std::vector<bob::entity_handle>& tag_handles = this->m_Sparse.handles();
			assert(tag_handles.size() == 4);
			
			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}

		void m_TestMiddle()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			const std::vector<bob::entity_handle>& tag_handles = this->m_Sparse.handles();

			for (size_t i = 0, n = tag_handles.size(); i < n; ++i)
			{
				const uint32_t dense_index = tag_handles[i].index();
				const bob::entity_handle handle = bob::entity_handle(static_cast<uint32_t>(i));

				assert(tag_handles[dense_index] == handle);
			}

			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}

		void m_TestComponentsThenRemove()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			const std::vector<TagB> tag_components = this->m_Sparse.components();
			assert(tag_components.size() == 4);

			for (size_t i = 0; i < 4; ++i)
			{
				const bob::entity_handle handle = bob::entity_handle(static_cast<uint32_t>(i));
				m_Sparse.remove(handle);
			}
			
			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}

		bob::sparse_set<TagB> m_Sparse;
};
#endif

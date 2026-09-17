/*
 * Copyright (c) 2026 Soo0707
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * Use of this software is governed by the zlib license. 
 * See the LICENSE file in the project root for full license text.
 */

#include <vector>

#include "catch_amalgamated.hpp"
#include "bob.hpp"

struct SparseSetFixture
{
	SparseSetFixture()
	{
		for (size_t i = 0; i < 4; i++)
		{
			const bob::entity handle = bob::entity(static_cast<uint32_t>(i));
			sparse_set.add(handle, i);
		}
	}

	bob::sparse_set<size_t> sparse_set;
};

TEST_CASE_METHOD(SparseSetFixture, "adding to a sparse set", "[sparse_set]")
{
	const bob::entity fifth = bob::entity(static_cast<uint32_t>(4));
	sparse_set.add(fifth, 4);

	const std::vector<bob::entity>& handles = sparse_set.handles();
	CHECK(handles.size() == 5);
}

TEST_CASE_METHOD(SparseSetFixture, "check if the middle layer lines up", "[sparse_set]")
{
	const std::vector<bob::entity>& handles = sparse_set.handles();

	for (size_t i = 0, n = handles.size(); i < n; ++i)
	{
		const uint32_t dense_index = handles[i].index();
		const bob::entity handle = bob::entity(static_cast<uint32_t>(i));

		CHECK(handles[dense_index] == handle);
	}
}

TEST_CASE_METHOD(SparseSetFixture, "shift entity 0 to the last location in dense layer", "[sparse_set]")
{
	const std::vector<bob::entity>& handles = sparse_set.handles();

	const bob::entity zero = bob::entity(0);
	const bob::entity last = bob::entity(static_cast<uint32_t>(handles.size() - 1));

	sparse_set.shift(zero, handles.size() - 1);

	CHECK(handles[0] == bob::entity(3));
	CHECK(handles[3] == zero);

	CHECK(sparse_set[zero] == 0);
	CHECK(sparse_set[last] == 3);
}

TEST_CASE_METHOD(SparseSetFixture, "remove component of an entity", "[sparse_set]")
{
	const std::vector<size_t>& components = sparse_set.components();

	const bob::entity handle = bob::entity(static_cast<uint32_t>(3));
	sparse_set.remove(handle);

	CHECK(components.size() == 3);
}

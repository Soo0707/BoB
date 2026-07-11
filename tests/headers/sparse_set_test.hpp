#include "bob/sparse_set.hpp"

void test_sparse_set()
{
	bob::sparse_set<Tag> tag_sparse = bob::sparse_set<Tag>();
	
	bob::entity_handle_generator tag_handle_generator = bob::entity_handle_generator();
	for (int i = 0; i < 4; i++)
	{
		const bob::entity_handle handle = tag_handle_generator.get_new_handle();
		tag_sparse.add(handle);
	}

	const std::vector<bob::entity_handle>& tag_handles = tag_sparse.get_handles();
	assert(tag_handles.size() == 4);

	for (uint32_t i = 0, n = static_cast<uint32_t>(tag_handles.size()); i < n; i++)
	{
		const uint32_t dense_index = tag_handles[i].index();
		const bob::entity_handle handle = bob::entity_handle(i);

		assert(tag_handles[dense_index] == handle);
	}

	const std::vector<Tag> tag_components = tag_sparse.get_components();
	assert(tag_components.size() == 4);

	for (uint32_t i = 0; i < 4; i++)
	{
		const bob::entity_handle handle = bob::entity_handle(i);
		tag_sparse.remove(handle);
	}
}

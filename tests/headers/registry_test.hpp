#include "bob/registry.hpp"


struct Vector2
{
	float x;
	float y;
};

struct Tag
{};

using sample_registry = bob::registry<Tag, Vector2, std::string>;

void test_registry()
{
	sample_registry r{};

	const bob::entity_handle first_handle = r.get_new_handle();
	assert(first_handle == bob::entity_handle(0));
	r.add<std::string>(first_handle, "first test string");


	const bob::entity_handle second_handle = r.get_new_handle();
	assert(second_handle == bob::entity_handle(1));
	r.add<Vector2>(second_handle, 6.0f, 7.0f);
	r.add<std::string>(second_handle, "second test string");


	const bob::entity_handle third_handle = r.get_new_handle();
	assert(third_handle == bob::entity_handle(2));
	r.add<Tag>(third_handle);
	r.add<Vector2>(third_handle, 12.0f, 14.0f);
	r.add<std::string>(third_handle, "third and last test string");

	const auto& string_set = r.get_sparse_set<std::string>();
	const auto& string_components = string_set.get_components();
	assert(string_components.size() == 3);

	const auto& vector_set = r.get_sparse_set<Vector2>();
	const auto& vector_components = vector_set.get_components();
	assert(vector_components.size() == 2);

	const auto& tag_set = r.get_sparse_set<Tag>();
	const auto& tag_components = tag_set.get_components();
	assert(tag_components.size() == 1);

	const auto& string_handles = r.get_iterator<std::string>();
	assert(string_handles.size() == 3);


	std::cout << "STRING ONLY" << "\n";
	for (auto i : string_handles)
		std::cout << i.index() << ": " << string_set[i] << std::endl;
	std::cout << "\n";


	const auto& string_vector_handles = r.get_iterator<Vector2, std::string>();
	assert(string_vector_handles.size() == 2);

	std::cout << "STRING AND VECTOR" << "\n";
	for (uint32_t i = 0, n = static_cast<uint32_t>(string_vector_handles.size()); i < n; i++)
	{
		const bob::entity_handle handle = string_vector_handles[i];

		std::cout << handle.index() << ": " <<
			string_set[handle] << ", "
			<< vector_set[handle].x << ", "
			<< vector_set[handle].y
			<< std::endl;
	}
	std::cout << "\n";


	const auto& string_vector_tag_handles = r.get_iterator<Tag, Vector2, std::string>();
	assert(string_vector_tag_handles.size() == 1);

	std::cout << "STRING VECTOR AND TAG\n";
	const bob::entity_handle handle = string_vector_tag_handles[0];
	std::cout
		<< handle.index()
		<< ": "
		<< string_set[handle]
		<< ", "
		<< vector_set[handle].x
		<< ", "
		<< vector_set[handle].y
		<< std::endl;
	std::cout << "\n";

	r.remove<Tag, Vector2, std::string>(third_handle);
	const auto& string_vector_tag_after_r_handles = r.get_iterator<Tag, Vector2, std::string>();
	assert(string_vector_tag_after_r_handles.size() == 0);

	r.remove<Vector2, std::string>(second_handle);
	const auto& string_vector_after_r_handles = r.get_iterator<Vector2, std::string>();
	assert(string_vector_after_r_handles.size() == 0);

	r.remove<std::string>(first_handle);
	const auto& string_after_r_handles = r.get_iterator<std::string>();
	assert(string_vector_after_r_handles.size() == 0);
}

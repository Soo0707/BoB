#include <iostream>
#include <cstdint>
#include <string>
#include <cassert>

#include "entity_handle.hpp"
#include "entity_handle_generator.hpp"
#include "sparse_set.hpp"
#include "registry.hpp"

struct Vector2
{
	float x;
	float y;
};

struct Tag
{};

void test_entity_handle()
{
	const bob::entity_handle zero_handle = bob::entity_handle(0);
	
	assert(zero_handle.index() == 0);
	assert(zero_handle.generation() == 0);
	assert(zero_handle.value() == 0);

	const bob::entity_handle six_handle = bob::entity_handle(6);

	assert(six_handle.index() == 6);
	assert(six_handle.generation() == 0);
	assert(six_handle.value() == 6);

	const bob::entity_handle seventh_generation = bob::entity_handle(0x700000);

	assert(seventh_generation.index() == 0);
	assert(seventh_generation.generation() == 7);
	assert(seventh_generation.value() == 0x700000);

	const bob::entity_handle mixed = bob::entity_handle(0x600007);

	assert(mixed.index() == 7);
	assert(mixed.generation() == 6);
	assert(mixed.value() == 0x600007);

	const bob::entity_handle first_same = bob::entity_handle(0x400005);
	const bob::entity_handle second_same = bob::entity_handle(0x400005);

	assert(first_same == second_same);
	
	const bob::entity_handle first_diff = bob::entity_handle(0x400005);
	const bob::entity_handle second_diff = bob::entity_handle(0x400006);

	assert(first_diff != second_diff);
}

void test_entity_handle_generator()
{
	bob::entity_handle_generator handle_generator = bob::entity_handle_generator();

	const bob::entity_handle first_handle = handle_generator.get_new_handle();
	assert(first_handle == bob::entity_handle(0));

	const bob::entity_handle second_handle = handle_generator.get_new_handle();
	assert(second_handle == bob::entity_handle(1));

	handle_generator.invalidate_handle(second_handle);

	const bob::entity_handle next_handle = handle_generator.peak_next_handle();
	assert(next_handle == bob::entity_handle(0x100001));

	const bob::entity_handle last_invalidated_handle = handle_generator.get_new_handle();
	assert(last_invalidated_handle == bob::entity_handle(0x100001));

	const bob::entity_handle third_handle = handle_generator.get_new_handle();
	assert(third_handle == bob::entity_handle(2));
}

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

	const std::vector<Tag> tag_components = tag_sparse.get_raw_components();
	assert(tag_components.size() == 4);

	for (uint32_t i = 0; i < 4; i++)
	{
		const bob::entity_handle handle = bob::entity_handle(i);
		tag_sparse.remove(handle);
	}
}

void test_registry()
{
	using registry_type = bob::registry<Tag, Vector2, std::string>;
	registry_type r{};

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
	const auto& string_components = string_set.get_raw_components();
	assert(string_components.size() == 3);

	const auto& vector_set = r.get_sparse_set<Vector2>();
	const auto& vector_components = vector_set.get_raw_components();
	assert(vector_components.size() == 2);

	const auto& tag_set = r.get_sparse_set<Tag>();
	const auto& tag_components = tag_set.get_raw_components();
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

int main(void)
{
	test_entity_handle();
	std::cout << "entity_handle tests passed!" << std::endl; 
	test_entity_handle_generator();
	std::cout << "entity_handle_generator tests passed!" << std::endl; 
	test_sparse_set();
	std::cout << "sparse_set tests passed!" << std::endl; 
	test_registry();
	std::cout << "registry tests didn't crash! check output." << std::endl; 
}

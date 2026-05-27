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
{
	const char * text = "ts a tag";
};

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

	const bob::handle_proxy tag_handle_proxy = tag_sparse.get_handles();
	assert(tag_handle_proxy.size == 4);

	for (uint32_t i = 0, n = tag_handle_proxy.size; i < n; i++)
	{
		const uint32_t dense_index = tag_handle_proxy.handles[i].index();
		const bob::entity_handle handle = bob::entity_handle(i);

		assert(tag_handle_proxy.handles[dense_index] == handle);
	}

	const bob::component_proxy<Tag> tag_component_proxy = tag_sparse.get_components();
	assert(tag_component_proxy.size == 4);

	for (uint32_t i = 0; i < 4; i++)
	{
		const bob::entity_handle handle = bob::entity_handle(i);
		tag_sparse.remove(handle);
	}
}

void test_registry()
{
	bob::registry<Tag, Vector2, std::string> r;

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


	const bob::component_proxy<std::string> string_components = r.get_component<std::string>();
	assert(string_components.size == 3);

	const bob::component_proxy<Vector2> vector_components = r.get_component<Vector2>();
	assert(vector_components.size == 2);

	const bob::component_proxy<Tag> tag_components = r.get_component<Tag>();
	assert(tag_components.size == 1);

	const bob::handle_proxy string_handle_proxy = r.get_handles<std::string>();
	assert(string_handle_proxy.size == 3);

	std::cout << "STRING ONLY" << "\n";
	for (uint32_t i = 0, n = string_handle_proxy.size; i < n; i++)
	{
		const uint32_t dense_index = string_handle_proxy.handles[i].index();
		std::cout << dense_index << ": " << string_components.components[dense_index] << std::endl;
	}
	std::cout << "\n";

	const bob::handle_proxy string_vector_handle_proxy = r.get_handles<Vector2, std::string>();
	assert(string_vector_handle_proxy.size == 2);

	std::cout << "STRING AND VECTOR" << "\n";
	for (uint32_t i = 0, n = string_vector_handle_proxy.size; i < n; i++)
	{
		const uint32_t dense_index = string_vector_handle_proxy.handles[i].index();
		std::cout << dense_index << ": " <<
			string_components.components[dense_index] << ", "
			<< vector_components.components[dense_index].x << ", "
			<< vector_components.components[dense_index].y
			<< std::endl;
	}
	std::cout << "\n";

	const bob::handle_proxy string_vector_tag_handle_proxy = r.get_handles<Tag, Vector2, std::string>();
	assert(string_vector_tag_handle_proxy.size == 1);

	std::cout << "STRING VECTOR AND TAG\n";
	const uint32_t dense_index = string_vector_tag_handle_proxy.handles[0].index();
	std::cout
		<< dense_index
		<< ": "
		<< string_components.components[dense_index]
		<< ", "
		<< vector_components.components[dense_index].x
		<< ", "
		<< vector_components.components[dense_index].y
		<< tag_components.components[dense_index].text
		<< std::endl;
	std::cout << "\n";

	r.remove<Tag, Vector2, std::string>(third_handle);
	r.remove<Vector2, std::string>(second_handle);
	r.remove<std::string>(first_handle);
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
	std::cout << "registry tests passed!" << std::endl; 
}

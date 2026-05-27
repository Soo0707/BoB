#include <iostream>
#include <cassert>
#include <string>

#include "entity_handle.hpp"
#include "entity_handle_generator.hpp"
#include "sparse_set.hpp"
#include "registry.hpp"

struct Vector2
{
	float x;
	float y;
}

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
	assert(zero_handle.value() == 6);

	const bob::entity_handle seventh_generation = bob::entity_handle(0x700000);

	assert(seventh_generation.index() == 0);
	assert(seventh_generation.generation() == 7);
	assert(zero_handle.value() == 0x700000);

	const bob::entity_handle mixed = bob::entity_handle(0x600007);

	assert(mix.index() == 7);
	assert(mix.generation() == 6);
	assert(mix.value() == 0x600007);

	const bob::entity_handle first_same = bob::entity_handle(0x400005);
	const bob::entity_handle second_same = bob::entity_handle(0x400005);

	assert(first_same == second_same);
	
	const bob::entity_handle first_diff = bob::entity_handle(0x400005);
	const bob::entity_handle second_diff = bob::entity_handle(0x400006);

	assert(first_diff != second_diff);
}

void test_entity_handle_generator()
{
	const bob::entity_handle_generator handle_generator = bob::entity_handle_generator();

	const bob::entity_handle first_handle = handle_generator.get_new_handle();
	assert(first_handle == bob::entity_handle(0));

	const bob::entity_handle second_handle = handle_generator.get_new_handle();
	assert(second_handle == bob::entity_handle(1));

	handle_generator.invalid_handle(second_handle);

	const bob::entity_handle next_handle = handle_generator.peak_next_handle();
	assert(next_handle == bob::entity_handle(0x100001));

	const bob::entity_handle last_invalidated_handle = handle_generator.get_new_handle();
	assert(last_invalidated_handle == bob::entity_handle(0x100001));

	const bob::entity_handle third_handle = handle_generator.get_new_handle();
	assert(third_handle == bob::entity_handle(3));
}

void test_sparse_set()
{
	bob::sparse_set<Tag> tag_sparse = bob::sparse_set<Tag>();
	
	tag_sparse.extend_sparse(2);
	for (int i = 0; i < 2; i++)
	{
		const uint32_t const* tag_sparse_outer = tag_sparse.sparse();
		assert(tag_sparse_outer[i] == bob::invalid_index);
	}

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

		assert[tag_component_proxy.handles[dense_index] == handle];
	}

	const bob::component_proxy<Tag> tag_component_proxy = tag_sparse.get_components();
	assert(tag_component_proxy.size == 4);

	for (uint32_t i = 0, n = tag_component_proxy.size; i < n; i++)
	{
		const Tag tag = tag_component_proxy.components[i];
		assert[tag == Tag()];
	}

	for (uint32_t i = 0; i < 4; i++)
	{
		const bob::entity_handle handle = bob::entity_handle(i);
		tag_sparse.remove(handle);
	}

	for (int i = 0; i < 4; i++)
	{
		const uint32_t const* tag_sparse_outer = tag_sparse.sparse();
		assert(tag_sparse_outer[i] == bob::invalid_index);
	}
}

void test_registry()
{
	using test_registry = registry<Tag, Vector2, std::string>;

	test_registry r = registry<Tag, Vector2, std::string>();

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

	for (uint32_t i = 0, n = string_handle_proxy.size; i < n; i++)
	{
		std::cout << "\n\n";
		const uint32_t dense_index = string_handle_proxy.handles[i].index();
		std::cout << dense_index << ": " << string_components.components[dense_index] << std::endl;
		std::cout << "\n\n";
	}

	const bob::handle_proxy string_vector_handle_proxy = r.get_handles<Vector2, std::string>();
	assert(string_vector_handle_proxy.size == 2);

	for (uint32_t i = 0, n = string_vector_handle_proxy.size; i < n; i++)
	{
		std::cout << "\n\n";
		const uint32_t dense_index = string_vector_handle_proxy.handles[i].index();
		std::cout << dense_index << ": " <<
			string_components.components[dense_index] << ", " << vector_components.components[dense_index]
			<< std::endl;
		std::cout << "\n\n";
	}

	const bob::handle_proxy string_vector_tag_handle_proxy = r.get_handles<Tag, Vector2, std::string>();
	assert(string_vector_tag_handle_proxy.size == 1);

	std::cout << "\n\n";
	const uint32_t dense_index = string_vector_tag_handle_proxy.handles[i].index();
	std::cout << dense_index << ": " <<
		string_components.components[dense_index] << ", " << vector_components.components[dense_index] <<
		<< std::endl;

	std::cout << "\n\n";

	r.remove<Tag, Vector2, std::string>(third_handle);
	r.remove<Vector2, std::string>(second_handle);
	r.remove<std::string>(first_handle);
}

int main(void)
{
	test_entity_handle();
	test_entity_handle_generator();
	test_sparse_set();
	test_registry();
}

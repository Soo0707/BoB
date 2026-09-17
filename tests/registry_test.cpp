/*
 * Copyright (c) 2026 Soo0707
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * Use of this software is governed by the zlib license. 
 * See the LICENSE file in the project root for full license text.
 */

#include <string>
#include <vector>

#include "catch_amalgamated.hpp"
#include "bob.hpp"

struct Vector2
{
	float x;
	float y;
};

struct Tag
{};

struct UnseededRegistryFixture
{
	UnseededRegistryFixture()
	{
		registry.register_component<std::string>();
		registry.register_component<Vector2>();
		registry.register_component<Tag>();
	}

	bob::registry registry;
};

struct SeededRegistryFixture
{
	SeededRegistryFixture()
	{
		registry.register_component<std::string>();
		registry.register_component<Vector2>();
		registry.register_component<Tag>();

		const bob::entity first = registry.create_handle();
		CHECK(first == bob::entity(0));

		const bob::entity second = registry.create_handle();
		CHECK(second == bob::entity(1));

		const bob::entity third = registry.create_handle();
		CHECK(third == bob::entity(2));

		registry.add<std::string>(first, "0");

		registry.add<std::string>(second, "1");
		registry.add<Vector2>(second, 6.0f, 7.0f);

		registry.add<std::string>(third, "2");
		registry.add<Vector2>(third, 12.0f, 14.0f);
		registry.add<Tag>(third);
	}

	bob::registry registry;
};

struct GroupedRegistryFixture
{
	GroupedRegistryFixture()
	{
		registry.register_group<Vector2, int>();

		// fill the int sparse set so there is discontinuity
		for (int i = 0; i < 10; ++i)
			registry.add<int>(bob::entity(i), i);

		registry.add<Vector2>(bob::entity(7), 12.0f, 14.0f);
		registry.add<Vector2>(bob::entity(3), 6.0f, 7.0f);
		registry.add<Vector2>(bob::entity(9), 6.0f, 7.0f);
	}

	bob::registry registry;
};

TEST_CASE_METHOD(UnseededRegistryFixture, "reserve() correctly reserves capacity of vectors", "[registry]")
{
	registry.reserve<std::string>(1024);
	registry.reserve<Vector2>(1024);
	registry.reserve<Tag>(1024);

	const size_t strings_capacity = registry.container<std::string>().components().capacity();
	const size_t vector2s_capacity = registry.container<Vector2>().components().capacity();
	const size_t tags_capacity = registry.container<Tag>().components().capacity();

	CHECK(strings_capacity >= 1024);
	CHECK(vector2s_capacity >= 1024);
	CHECK(tags_capacity >= 1024);
}

TEST_CASE_METHOD(UnseededRegistryFixture, "add a std::string to entity 0", "[registry]")
{
	const bob::entity first = registry.create_handle();
	CHECK(first == bob::entity(0));

	registry.add<std::string>(first, "0");
	
	const size_t string_set_size = registry.container<std::string>().components().size();
	CHECK(string_set_size == 1);
}

TEST_CASE_METHOD(UnseededRegistryFixture, "add correctly adds a component to an entity", "[registry]")
{
	const bob::entity first = registry.create_handle();
	CHECK(first == bob::entity(0));

	const bob::entity second = registry.create_handle();
	CHECK(second == bob::entity(1));

	const bob::entity third = registry.create_handle();
	CHECK(third == bob::entity(2));

	registry.add<std::string>(first, "0");

	registry.add<std::string>(second, "1");
	registry.add<Vector2>(second, 6.0f, 7.0f);

	registry.add<std::string>(third, "2");
	registry.add<Vector2>(third, 12.0f, 14.0f);
	registry.add<Tag>(third);

	const bob::sparse_set<std::string>& strings = registry.container<std::string>();
	const bob::sparse_set<Vector2>& vector2s = registry.container<Vector2>();
	const bob::sparse_set<Tag>& tags = registry.container<Tag>();

	CHECK(strings.has(first));
	CHECK_FALSE(vector2s.has(first));
	CHECK_FALSE(tags.has(first));

	CHECK(strings.has(second));
	CHECK(vector2s.has(second));
	CHECK_FALSE(tags.has(second));

	CHECK(strings.has(third));
	CHECK(vector2s.has(third));
	CHECK(tags.has(third));
}

TEST_CASE_METHOD(SeededRegistryFixture, "iterate through entities with std::string", "[registry]")
{
	const std::vector<bob::entity>& iterator = registry.iterator<std::string>();
	CHECK(iterator.size() == 3);

	const bob::sparse_set<std::string>& strings = registry.container<std::string>();

	for (size_t i = 0, n = iterator.size(); i < n; ++i)
	{
		const bob::entity handle = iterator[i];

		CHECK(strings[handle] == std::to_string(i));
	}
}

TEST_CASE_METHOD(SeededRegistryFixture, "iterate through entities with std::string and Vector2", "[registry]")
{
	const auto& iterator = registry.iterator<Vector2, std::string>();
	CHECK(iterator.size() == 2);

	const bob::sparse_set<std::string>& strings = registry.container<std::string>();
	const bob::sparse_set<Vector2>& vectors = registry.container<Vector2>();

	for (size_t i = 0, n = iterator.size(); i < n; ++i)
	{
		const bob::entity handle = iterator[i];

		CHECK(strings[handle] == std::to_string(i + 1));
		CHECK(vectors[handle].x == static_cast<float>((i + 1) * 6.0f));
		CHECK(vectors[handle].y == static_cast<float>((i + 1) * 7.0f));
	}
}

TEST_CASE_METHOD(SeededRegistryFixture, "iterate through entities with all components", "[registry]")
{
	const auto& iterator = registry.iterator<Tag, Vector2, std::string>();
	CHECK(iterator.size() == 1);

	const bob::sparse_set<std::string>& strings = registry.container<std::string>();
	const bob::sparse_set<Vector2>& vectors = registry.container<Vector2>();

	const bob::entity handle = iterator[0];

	CHECK(strings[handle] == "2");
	CHECK(vectors[handle].x == 12.0f);
	CHECK(vectors[handle].y == 14.0f);
}

TEST_CASE_METHOD(SeededRegistryFixture, "remove entity 2 and ensure recycled entity handle", "[registry]")
{
	const bob::entity third = bob::entity(2);

	registry.remove<Tag, Vector2, std::string>(third);
	registry.release_handle(third);

	const std::vector<bob::entity>& iterator = registry.iterator<Tag, Vector2, std::string>();
	CHECK(iterator.size() == 0);

	const bob::entity next = registry.create_handle();
	CHECK(next.generation() == 1);
	CHECK(next.index() == 2);
}

TEST_CASE_METHOD(SeededRegistryFixture, "remove std::string from entity 1", "[registry]")
{
	const bob::entity second = bob::entity(1);

	registry.remove<std::string>(second);
	const bob::sparse_set<std::string>& strings = registry.container<std::string>();
	CHECK_FALSE(strings.has(second));

	const std::vector<bob::entity>& iterator = registry.iterator<std::string>();
	CHECK(iterator.size() == 2);

	for (const auto handle : iterator)
		CHECK(handle != second);
}

TEST_CASE_METHOD(GroupedRegistryFixture, "add to a group and ensures it orders the front of grouped sparse sets", "[registry]")
{
	const bob::group<Vector2, int>& group = registry.containers<Vector2, int>();

	registry.add<Vector2>(bob::entity(2), 6.0f, 7.0f);
	CHECK(group.size() == 4);

	const std::vector<bob::entity>& vector2_handles = registry.container<Vector2>().handles();
	const std::vector<bob::entity>& int_handles = registry.container<int>().handles();

	for (size_t i = 0, n = group.size(); i < n; ++i)
		CHECK(vector2_handles[i] == int_handles[i]);
}

TEST_CASE_METHOD(GroupedRegistryFixture, "remove from a group and ensures it orders the front of grouped sparse sets", "[registry]")
{
	const bob::group<Vector2, int>& group = registry.containers<Vector2, int>();

	registry.remove<int>(bob::entity(3));
	
	const std::vector<bob::entity>& vector2_handles = registry.container<Vector2>().handles();
	const std::vector<bob::entity>& int_handles = registry.container<int>().handles();

	for (size_t i = 0, n = group.size(); i < n; ++i)
		CHECK(vector2_handles[i] == int_handles[i]);

	CHECK(group.size() == 2);
}

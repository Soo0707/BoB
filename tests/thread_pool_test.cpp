/*
 * Copyright (c) 2026 Soo0707
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * Use of this software is governed by the zlib license. 
 * See the LICENSE file in the project root for full license text.
 */

#include <cstdint>
#include <thread>
#include <vector>

#include "catch_amalgamated.hpp"
#include "bob.hpp"

struct ThreadPoolFixture
{
	ThreadPoolFixture() :
		pool(std::thread::hardware_concurrency())
	{
		for (size_t i = 0; i < 1048575; ++i)
			data.emplace_back(i);
	}

	bob::thread_pool pool;
	std::vector<size_t> data;
};

struct GroupThreadPoolFixture
{
	GroupThreadPoolFixture() :
		pool(std::thread::hardware_concurrency())
	{
		registry.register_group<uint32_t, size_t>();

		for (size_t i = 0; i < 1048575; ++i)
		{
			const bob::entity handle = bob::entity(static_cast<uint32_t>(i));

			registry.add<uint32_t>(handle, static_cast<uint32_t>(i));
			registry.add<size_t>(handle, i);
		}
	}

	bob::thread_pool pool;
	bob::registry registry;
};

// separate the benchmark because BENCHMARK runs the function multiple times
// running it multiple times will make CHECK fail because the numbers have been doubled more than once
TEST_CASE_METHOD(ThreadPoolFixture, "run parallelise", "[thread_pool]")
{
	pool.parallelise(data, [](size_t& n){ n *= 2; }, 1);
	for (size_t i = 0, n = data.size(); i < n; ++i)
		CHECK(data[i] == 2 * i);
}

TEST_CASE_METHOD(ThreadPoolFixture, "benchmark non-grouped parallelise", "[thread_pool]")
{
	BENCHMARK("parallelise")
	{
		pool.parallelise(data, [](size_t& n){ n *= 2; }, 1);
	};
}

TEST_CASE_METHOD(GroupThreadPoolFixture, "run grouped parallelise", "[thread_pool]")
{
	bob::group<uint32_t, size_t>& group = registry.containers<uint32_t, size_t>();

	auto func = [](std::vector<uint32_t>& ints, std::vector<size_t>& sizes, const size_t i)
	{ sizes[i] += ints[i]; };

	pool.parallelise(group, func, 1);

	const std::vector<size_t>& sizes = registry.container<size_t>().components();
	const std::vector<uint32_t>& ints = registry.container<uint32_t>().components();

	for (size_t i = 0, n = sizes.size(); i < n; ++i)
		CHECK(sizes[i] == 2 * ints[i]);
}

TEST_CASE_METHOD(GroupThreadPoolFixture, "benchmark grouped parallelise", "[thread_pool]")
{
	bob::group<uint32_t, size_t>& group = registry.containers<uint32_t, size_t>();

	auto func = [](std::vector<uint32_t>& ints, std::vector<size_t>& sizes, const size_t i)
	{ sizes[i] += ints[i]; };

	BENCHMARK("parallelise")
	{
		pool.parallelise(group, func, 1);
	};
}

TEST_CASE("single thread baseline", "[thread_pool]")
{
	std::vector<size_t> data;

	for (size_t i = 0; i < 1048575; ++i)
		data.emplace_back(i);

	BENCHMARK("single thread")
	{
		for (size_t i = 0, n = data.size(); i < n; ++i)
			data[i] *= 2;
	};
}

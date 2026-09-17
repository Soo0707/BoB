/*
 * Copyright (c) 2026 Soo0707
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * Use of this software is governed by the zlib license. 
 * See the LICENSE file in the project root for full license text.
 */

#include "catch_amalgamated.hpp"
#include "bob.hpp"

TEST_CASE("create an entity of index 0 and generation 0", "[entity]")
{
	const bob::entity zero_handle = bob::entity(0);
	
	CHECK(zero_handle.index() == 0);
	CHECK(zero_handle.generation() == 0);
	CHECK(zero_handle.value() == 0);
}

TEST_CASE("create an entity of index 6 and generation 0", "[entity]")
{
	const bob::entity six_handle = bob::entity(6);

	CHECK(six_handle.index() == 6);
	CHECK(six_handle.generation() == 0);
	CHECK(six_handle.value() == 6);
}

TEST_CASE("create an entity of index 0 and generation 7", "[entity]")
{
	const bob::entity seventh_generation = bob::entity(0x700000);

	CHECK(seventh_generation.index() == 0);
	CHECK(seventh_generation.generation() == 7);
	CHECK(seventh_generation.value() == 0x700000);
}

TEST_CASE("create an entity of index 7 and generation 6", "[entity]")
{
	const bob::entity mixed = bob::entity(0x600007);

	CHECK(mixed.index() == 7);
	CHECK(mixed.generation() == 6);
	CHECK(mixed.value() == 0x600007);
}

TEST_CASE("check operator==() of entity", "[entity]")
{
	const bob::entity first_handle = bob::entity(0x400005);
	const bob::entity second_handle = bob::entity(0x400005);

	CHECK(first_handle == second_handle);
}

TEST_CASE("check operator!=() of entity", "[entity]")
{
	const bob::entity first_handle = bob::entity(0x400005);
	const bob::entity second_handle = bob::entity(0x400006);

	CHECK(first_handle != second_handle);
}

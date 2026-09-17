/*
 * Copyright (c) 2026 Soo0707
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * Use of this software is governed by the zlib license. 
 * See the LICENSE file in the project root for full license text.
 */

#include "catch_amalgamated.hpp"
#include "bob.hpp"

struct HandleGeneratorFixture
{
	bob::handle_generator generator;
};

TEST_CASE_METHOD(HandleGeneratorFixture, "generate the zero handle", "[handle_generator]")
{
	const bob::entity first_handle = generator.create_handle();
	CHECK(first_handle == bob::entity(0));
}

TEST_CASE_METHOD(HandleGeneratorFixture, "generate the one handle", "[handle_generator]")
{
	const bob::entity first_handle = generator.create_handle();
	const bob::entity second_handle = generator.create_handle();

	CHECK(second_handle == bob::entity(1));
}

TEST_CASE_METHOD(HandleGeneratorFixture, "invalidate a handle then request another for a generation bump", "[handle generator]")
{
	const bob::entity handle = generator.create_handle();
	generator.invalidate_handle(handle);

	const bob::entity recycled_handle = generator.create_handle();
	CHECK(recycled_handle == bob::entity(0x100000));
}

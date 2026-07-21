/*
 * Copyright (c) 2026 Soo0707
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * Use of this software is governed by the zlib license. 
 * See the LICENSE file in the project root for full license text.
 */

#include "headers/entity_handle_test.hpp"
#include "headers/entity_handle_generator_test.hpp"
#include "headers/sparse_set_test.hpp"
#include "headers/registry_test.hpp"
#include "headers/thread_pool_test.hpp"

int main(void)
{
	EntityHandleTest a{};
	EntityHandleGeneratorTest b{};

	SparseSetTest c{};
	RegistryTest d{};

	ThreadPoolTest e{};
}

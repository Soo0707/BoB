/*
 * Copyright (c) 2026 Soo0707
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * Use of this software is governed by the zlib license. 
 * See the LICENSE file in the project root for full license text.
 */

#ifndef BOB_HANDLE_GENERATOR_TEST
#define BOB_HANDLE_GENERATOR_TEST

#include "bob/handle_generator.hpp"

#include <iostream>
#include <cassert>

class EntityHandleGeneratorTest
{
	public:
		EntityHandleGeneratorTest()
		{
			this->m_FirstHandle();
			this->m_SecondHandle();
			this->m_InvalidateThenRecycle();
		}

	private:
		void m_FirstHandle()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			const bob::entity first_handle = this->m_HandleGenerator.create_handle();
			assert(first_handle == bob::entity(0));
			
			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}
		
		void m_SecondHandle()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";
			
			const bob::entity second_handle = this->m_HandleGenerator.create_handle();
			assert(second_handle == bob::entity(1));
			
			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}

		void m_InvalidateThenRecycle()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			const bob::entity second_handle = bob::entity(1);

			this->m_HandleGenerator.invalidate_handle(second_handle);

			const bob::entity recycled_handle = this->m_HandleGenerator.create_handle();
			assert(recycled_handle == bob::entity(0x100001));
			
			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}
		
		// TODO: test recycling from generation 4096

		bob::handle_generator m_HandleGenerator;
};

#endif

#ifndef BOB_ENTITY_HANDLE_TEST
#define BOB_ENTITY_HANDLE_TEST

#include "bob/entity_handle.hpp"

#include <iostream>
#include <cassert>

class EntityHandleTest
{
	public:
		EntityHandleTest()
		{
			this->m_ZeroHandle();
			this->m_SixHandle();
			this->m_SeventhGeneration();
			this->m_Mixed();
			this->m_Equality();
			this->m_Inequality();
		}

	private:
		void m_ZeroHandle()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			const bob::entity_handle zero_handle = bob::entity_handle(0);
			
			assert(zero_handle.index() == 0);
			assert(zero_handle.generation() == 0);
			assert(zero_handle.value() == 0);
			
			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}
		
		void m_SixHandle()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";
			
			const bob::entity_handle six_handle = bob::entity_handle(6);

			assert(six_handle.index() == 6);
			assert(six_handle.generation() == 0);
			assert(six_handle.value() == 6);
			
			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}

		void m_SeventhGeneration()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			const bob::entity_handle seventh_generation = bob::entity_handle(0x700000);

			assert(seventh_generation.index() == 0);
			assert(seventh_generation.generation() == 7);
			assert(seventh_generation.value() == 0x700000);

			
			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}

		void m_Mixed()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			const bob::entity_handle mixed = bob::entity_handle(0x600007);

			assert(mixed.index() == 7);
			assert(mixed.generation() == 6);
			assert(mixed.value() == 0x600007);
			
			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}

		void m_Equality()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			const bob::entity_handle first_same = bob::entity_handle(0x400005);
			const bob::entity_handle second_same = bob::entity_handle(0x400005);

			assert(first_same == second_same);

			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}

		void m_Inequality()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			const bob::entity_handle first_diff = bob::entity_handle(0x400005);
			const bob::entity_handle second_diff = bob::entity_handle(0x400006);

			assert(first_diff != second_diff);
			
			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}
};

#endif

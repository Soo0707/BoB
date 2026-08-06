/*
 * Copyright (c) 2026 Soo0707
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * Use of this software is governed by the zlib license. 
 * See the LICENSE file in the project root for full license text.
 */

#ifndef BOB_THREAD_POOL_TEST
#define BOB_THREAD_POOL_TEST

#include <iostream>
#include <cstddef>
#include <vector>
#include <chrono>

#include "bob/bob.hpp"

class ThreadPoolTest
{
	public:
		ThreadPoolTest() :
			m_Pool(std::thread::hardware_concurrency())
		{
			this->m_TestParallelise();
			this->m_TestParalleliseGrouped();
			this->m_TestSingle();
		}

	private:
		void m_TestParallelise()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			std::vector<size_t> data_vector;

			for (size_t i = 0; i < 1048575; ++i)
				data_vector.emplace_back(i);

			auto start = std::chrono::high_resolution_clock::now();
	
			this->m_Pool.parallelise(data_vector, [](size_t& data){ data *= 2; }, 1);

			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

			std::cout << "Chunking took " << duration << "\n"; 

			for (size_t i = 0, n = data_vector.size(); i < n; ++i)
				assert(data_vector[i] == 2 * i);

			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}

		void m_TestParalleliseGrouped()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			bob::registry r;

			r.register_group<int, size_t>();

			for (size_t i = 0; i < 1048575; ++i)
			{
				r.add<int>(bob::entity_handle(static_cast<uint32_t>(i)), static_cast<int>(i));
				r.add<size_t>(bob::entity_handle(static_cast<uint32_t>(i)), i);
			}

			bob::group<int, size_t>& test_group = r.containers<int, size_t>();
			
			auto func = [](std::vector<int>& ints, std::vector<size_t>& sizes, const size_t i){
				sizes[i] += ints[i];
			};
	
			auto start = std::chrono::high_resolution_clock::now();

			this->m_Pool.parallelise(test_group, func, 1);

			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
			
			std::vector<size_t>& data = r.container<size_t>().components();
			for (size_t i = 0, n = data.size(); i < n; ++i)
				assert(data[i] == 2 * i);

			std::cout << "Grouped chunking took " << duration << "\n";
			std::cout << "NOTE: execution time becomes comparable without debug and sanitisation compile flags\n";

			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";

		}

		void m_TestSingle()
		{
			std::cout << __FILE_NAME__ << ": Running " << __FUNCTION__ << "\n";

			std::vector<size_t> data_vector;

			for (size_t i = 0; i < 1048575; ++i)
				data_vector.emplace_back(i);

			auto start = std::chrono::high_resolution_clock::now();
	
			for (size_t i = 0, n = data_vector.size(); i < n; ++i)
				data_vector[i] *= 2;

			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

			std::cout << "Single threaded took " << duration << "\n"; 

			std::cout << __FILE_NAME__ << ": " << __FUNCTION__ << " passed\n";
		}

		bob::thread_pool m_Pool;
};

#endif

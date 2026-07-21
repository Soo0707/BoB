/*
 * Copyright (c) 2026 Soo0707
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * Use of this software is governed by the zlib license. 
 * See the LICENSE file in the project root for full license text.
 */

#ifndef BOB_THREAD_POOL_TEST
#define BOB_THREAD_POOL_TEST

#include "bob/thread_pool.hpp"

#include <iostream>
#include <vector>
#include <chrono>

class ThreadPoolTest
{
	public:
		ThreadPoolTest() :
			m_Pool(std::thread::hardware_concurrency())
		{
			this->m_TestParallelise();
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

#include "bob/thread_pool.hpp"

using thread_registry = bob::registry<uint32_t>;

void test_thread_pool()
{
	thread_registry r{};
	bob::thread_pool pool = bob::thread_pool(std::thread::hardware_concurrency());

	for (size_t i = 0; i < 1048575; ++i)
	{
		const bob::entity_handle handle = r.get_new_handle();
		r.add<uint32_t>(handle, i);
	}

	auto& data_sparse_set = r.get_sparse_set<uint32_t>();
	auto& dense_layer = data_sparse_set.get_components();

	auto start = std::chrono::high_resolution_clock::now();
	pool.parallelise(dense_layer, [](uint32_t& data, size_t i){ data *= 2; }, 1);
	
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Function took " << duration.count() << " ms to complete.\n";

	for (size_t i = 0, n = dense_layer.size(); i < n; ++i)
		assert(dense_layer[i] == 2 * i && "data was not modified");
}

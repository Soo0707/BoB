#include <iostream>
#include <cstdint>
#include <string>
#include <cassert>
#include <chrono>
:
int main(void)
{
	test_entity_handle();
	std::cout << "entity_handle tests passed!" << std::endl; 
	test_entity_handle_generator();
	std::cout << "entity_handle_generator tests passed!" << std::endl; 
	test_sparse_set();
	std::cout << "sparse_set tests passed!" << std::endl; 
	test_registry();
	std::cout << "registry tests didn't crash! check output." << std::endl;
	test_thread_pool();
	std::cout << "thread_pool tests passed!" << std::endl; 
}

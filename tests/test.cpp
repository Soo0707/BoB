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

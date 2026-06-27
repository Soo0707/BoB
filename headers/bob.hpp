#ifndef BOB
#define BOB

#include <string>

#include "entity_handle.hpp"
#include "entity_handle_generator.hpp"
#include "sparse_set.hpp"
#include "registry.hpp"
#include "thread_pool.hpp"

struct Vector2
{
	float x;
	float y;
};

struct Tag
{};

using sample_registry = bob::registry<Tag, Vector2, std::string>;
using thread_registry = bob::registry<uint32_t>;
#endif

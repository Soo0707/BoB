#ifndef BOB
#define BOB

#include <string>

#include "bob/entity_handle_generator.hpp"
#include "bob/entity_handle.hpp"
#include "bob/registry.hpp"
#include "bob/sparse_set.hpp"
#include "bob/thread_pool.hpp"


// include or define your component headers below.

struct Vector2
{
	float x;
	float y;
};

struct Tag
{};


/*
 * create a type alias for your registry(s) below.
 * 
 * using sample_registry = bob::registry<ComponentTypeA, ComponentTypeB, ComponentTypeC>;
 */	

using sample_registry = bob::registry<Tag, Vector2, std::string>;
using thread_registry = bob::registry<uint32_t>;
#endif

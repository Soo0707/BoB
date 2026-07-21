<h1 align="center">BoB</h1>

BoB (Bundle of Boilerplate) is a simple, barebones, header-only sparse set-based Entity Component System written in C++20.

## Why BoB?

The goal of BoB is to be as simple, and as transparant as possible. You are encouraged to modify BoB's components to suit your needs if necessary.

## Define "transparant"?

Every component of BoB is built upon functionality that the C++ standard library provides. There are no complex iterators or fancy intermediate wrapper objects. Unless necessary, everything handed to you is a `std::vector`.

## Why use an ECS?

There are many [advantages of using an ECS](https://github.com/SanderMertens/ecs-faq) already highlighted by the author of Flecs.

I believe an ECS promotes composition of simple plain old data components and the placement of said components in a contiguous manner in memory which entails

- **Better Cache Locality:** Components are small and their sequential order in memory all but guarantees cache hits
- **Aggressive Compiler Optimisations:** Having less fluff around an object means the compiler can more aggressively make use of SIMD and other optimisations

## What does BoB offer?

BoB consists of:

- Entity Handle
- Entity Handle Generator
- Sparse Set
- Registry
- Thread Pool

## How do I get started?

This assumes a certain project folder structure.

1. Place the contents of `include/bob` into the root folder of your project
1. Include `bob/bob.hpp` in any of your related project files

## Example

```c++
#include <string>

#include "bob/bob.hpp"
#include "example_components.hpp"

int main()
{
    bob::registry registry;

    // components must be registered first
    registry.register_component<Vector2>();
    registry.register_component<PlayerTag>();

    // to create an entity, request for an entity_handle;
    const bob::entity_handle entity_zero = registry.create_handle();

    // add a Vector2 { 6.0f, 7.0f } component to the entity
    registry.add<Vector2>(entity_zero, 6.0f, 7.0f);

    // add a PlayerTag to the entity
    registry.add<PlayerTag>(entity_zero);

    // get sparse set containing Vector2
    bob::sparse_set<Vector2>& vector2_set = registry.container<Vector2>();
    
    // get sparse set containing PlayerTag
    bob::sparse_set<PlayerTag>& player_set = registry.container<PlayerTag>();

    // get dense layer of Vector2 sparse set
    std::vector<Vector2>& vectors = vector2_set.components();
    
    // cache friendly iteration of components
    for (auto& v : vectors)
    {
        v.x += 6.0f;
        v.y += 7.0f;
    }

    // get entities that contain BOTH Vector2 and PlayerTag
    const std::vector<bob::entity_handle>& iter = registry.iterator<Vector2, PlayerTag>();

    // accessing components through entity handles, use sparingly
    for (const bob::entity_handle h : iter)
    {
        // O(1) per lookup using operator[] but is not cache friendly
        // involves jumping from sparse to dense layer

        Vector2& vec = vector2_set[h];
        PlayerTag& tag = player_set[h];
    }

    // remove component(s) from an entity
    registry.remove<Vector2, PlayerTag>(entity_zero);

    // recycle the entity handle
    registry.release_handle(entity_zero);
}

```

## Cheatsheet

**Namespace:** `bob::entity_handle`

```c++
// constructor
entity_handle(const uint32_t) 

// returns generation of handle
uint32_t generation() const 

// returns dense layer index of entity
uint32_t index() const

// returns raw value of entity handle
uint32_t value() const
```

**Namespace:** `bob::sparse_set`

```c++
// NOTE: sparse_set is not copyable. only movable.

// returns if an entity is in the sparse set
bool has(const entity_handle) const

// returns pointer to T if an entity is in the sparse set and nullptr otherwise
T* try_get(const entity_handle)

// returns reference to T by using operator[] and an entity handle
T& operator[](const entity_handle handle)

// returns reference to component layer
std::vector<T>& components()

// returns reference to handle layer
const std::vector<entity_handle>& handles() const
```

**Namespace:** `bob::registry`

```c++
// register a component
void register_component<T>();

// returns next successive entity handle
entity_handle create_handle()

// recycle entity handle
void release_handle(const entity_handle handle)

// add component T to entity. args = arguments to construct T
void add<T>(const entity_handle handle, Arg&&... args)

// removes all components specified in T of an entity
void remove<T...>(const entity_handle handle)

// First and After are a list of components
// returns the entity handle layer of the smallest sparse set
// which contains all components specified in First and After
const std::vector<entity_handle>& iterator<First, After...>() const

// returns a sparse set of component type T
sparse_set<T>& container<T>()
```

**Namespace:** `bob::thread_pool`

```c++

// constructor. n = number of worker threads
thread_pool(const size_t n)

// chunk process a vector when its size is larger than grain
// callback function must have signature F(T& in) and should
// contain the transformation to be applied for each element
// this function is blocking
void parallelise(std::vector<T>& data, F callback, const size_t grain = 20000)
```

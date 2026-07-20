<h1 align="center">BoB</h1>

BoB (Bundle of Boilerplate) is a simple, barebones, header only sparse set-based Entity Component System written in C++20.

# Why BoB?

The goal of BoB is to be as simple, and as transparant as possible. You are encouraged to modify BoB's components to suit your needs.

## Define "transparant"?

Every component of BoB is built upon functionality that the C++ standard library provides. There are no complex iterators or fancy intermediate wrapper objects. Unless necessary, everything handed to you is a `std::vector`.

## Components

BoB consists of:

- Entity Handle
- Entity Handle Generator
- Sparse Set
- Registry
- Thread Pool

# How do I get started?

This assumes a certain project folder structure.

1. Place the contents of `include/bob` into the root folder of your project
1. Include `bob/bob.hpp` in any of your related project files

# Example

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
    const bob::entity_handle entity_zero = registry.get_new_handle();

    // add a Vector2 { 6.0f, 7.0f } component to the entity
    registry.add<Vector2>(entity_zero, 6.0f, 7.0f);

    // add a PlayerTag to the entity
    registry.add<PlayerTag>(entity_zero);

    // get sparse set containing Vector2
    bob::sparse_set<Vector2>& vector2_set = registry.get_sparse_set<Vector2>();

    // get sparse set containing PlayerTag
    bob::sparse_set<PlayerTag>& player_set = registry.get_sparse_set<PlayerTag>();

    // get entities that contain BOTH Vector2 and PlayerTag
    const std::vector<bob::entity_handle>& iter = registry.get_iterator<Vector2, PlayerTag>();

    // accessing components through entity handles
    for (const bob::entity_handle h : iter)
    {
        Vector2 vec = vector2_set[h];
        PlayerTag tag = player_set[h];
    }

    // remove an entity and their components
    registry.remove<Vector2, PlayerTag>(entity_zero);
}

```

# Cheatsheet
The methods below should be all a normal user would need.

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
std::vector<T>& get_components()

// returns reference to handle layer
const std::vector<entity_handle>& get_handles() const
```

**Namespace:** `bob::registry`

```c++
// register a component
void register_component<T>();

// returns next successive entity handle
entity_handle get_new_handle()

// add component T to entity. args = arguments to construct T
void add<T>(const entity_handle handle, Arg&&... args)

// removes all components specified in T of an entity and recycle its handle
void remove<T...>(const entity_handle handle)

// First and After are a list of components
// returns the entity handle layer of the smallest sparse set
// which contains all components specified in First and After
const std::vector<entity_handle>& get_iterator<First, After...>() const

// returns a sparse set of component type T
sparse_set<T>& get_sparse_set<T>()
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

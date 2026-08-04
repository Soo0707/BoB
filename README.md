<h1 align="center">BoB</h1>

BoB (Bundle of Boilerplate) is a simple, barebones, header-only sparse set-based [Entity Component System](https://github.com/SanderMertens/ecs-faq) written in C++20.

## Why BoB?

The goal of BoB is to be as simple, and as transparant as possible. You are encouraged to modify BoB's components to suit your needs if necessary.

## Define "transparant"?

Every component of BoB is built upon functionality that the C++ standard library provides. There are no complex iterators or fancy intermediate wrapper objects. Unless necessary, everything handed to you is a `std::vector`.

## What does BoB offer?

- Generational Entity Handle
- Entity Handle Generator
- Sparse Set
- Groups
- Registry
- Thread Pool

## How do I get started?

This assumes a certain project folder structure.

1. Place the contents of `include/bob` into the root folder of your project
1. Include `bob/bob.hpp` in any of your related project files

## Example

```c++
#include "bob/bob.hpp"
#include "example_components.hpp"

int main()
{
    bob::registry registry;

    // components must be registered first
    registry.register_component<PlayerTag>();

    // performance critical components should be registered as a group
    // grouped components are still owned by the registry and can be treated the same as non-grouped sparse sets
    registry.register_group<Position, Velocity>();

    // to create an entity, request for an entity_handle;
    const bob::entity_handle entity_zero = registry.create_handle();

    // add a Position { 6.0f, 7.0f } component to the entity
    registry.add<Position>(entity_zero, 6.0f, 7.0f);

    // add a Velocity { 0.0f, 0.0f } component to the entity
    registry.add<Velocity>(entity_zero, 0.0f, 0.0f);

    // add a PlayerTag to the entity
    registry.add<PlayerTag>(entity_zero);

    // get sparse set containing Position
    bob::sparse_set<Position>& position_set = registry.container<Position>();
    
    // get group containing Position and Velocity
    bob::group<Position, Velocity>& movement_group = registry.containers<Position, Velocity>();

    // get sparse set containing Velocity from a group
    bob::sparse_set<Velocity>& velocity_set = movement_group.container<Velocity>();
    
    // get dense layer of the Position sparse set
    std::vector<Position>& positions = position_set.components();

    // get dense layer of the Velociy sparse set
    std::vector<Velocity>& velocity = velocity_set.components();

    // since Position and Velocity are part of the same group
    // their dense layers are perfect SoA
    for (size_t i = 0, n = movement_group.size(); i < n; ++i)
    {
        positions[i].x += velocity[i].x;
        positions[i].y += velocity[i].y;
    }

    // get sparse set containing PlayerTag
    bob::sparse_set<PlayerTag>& player_set = registry.container<PlayerTag>();

    // get entities that contain BOTH Position and PlayerTag
    const std::vector<bob::entity_handle>& iter = registry.iterator<Position, PlayerTag>();

    // accessing components through entity handles, use sparingly
    for (const bob::entity_handle h : iter)
    {
        // O(1) per lookup using operator[] but is not cache friendly
        // involves jumping from sparse to dense layer

        Position& vec = position_set[h];
        PlayerTag& tag = player_set[h];
    }

    // remove component(s) from an entity
    registry.remove<Position, Velocity, PlayerTag>(entity_zero);

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

// returns reference to T by using operator[] and an entity handle
T& operator[](const entity_handle handle)

// returns reference to component layer
std::vector<T>& components()

// returns reference to handle layer
const std::vector<entity_handle>& handles() const

// reserve memory for the handle and component layer
void reserve(const size_t new_size)
```

**Namespace:** `bob::group`

```c++
// NOTE: a group does not own the sparse sets
// groups implements entt-like groups but does not support subgroups

// returns a sparse set of T as long as T is part of the group
sparse_set<T>& container()

// returns the size of the group
size_t size()
```

**Namespace:** `bob::registry`

```c++
// register a component
void register_component<T>()

// register a group of pack T...
void register_group<T...>()

// returns next successive entity handle
entity_handle create_handle()

// recycle entity handle, does not check for duplicates
// susceptible to double free-like errors
void release_handle(const entity_handle handle)

// add component T to entity. args = arguments to construct T
void add<T>(const entity_handle handle, Arg&&... args)

// removes all components specified in T of an entity
void remove<T...>(const entity_handle handle)

// calls reserve for all component types specified in pack T
void reserve<T...>(const size_t new_capacity)

// First and After are a list of components
// returns the entity handle layer of the smallest sparse set
// which contains all components specified in First and After
const std::vector<entity_handle>& iterator<First, After...>() const

// returns a sparse set of component type T
sparse_set<T>& container<T>()

// returns a registered group of pack T...
group<T...>& containers<T...>()
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

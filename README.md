<h1 align="center">BoB</h1>

**BoB** (Bundle of Boilerplate) is a simple, barebones, header only sparse set-based Entity Component System written in **C++20**.

# Why BoB?

The goal of **BoB** is to be as simple, and as transparant as possible. You are encouraged to modify **BoB**'s components to suit your needs.

## Define "transparant"?

Every component of **BoB** is built upon functionality that the C++ standard library provides. There are no complex iterators or fancy intermediate wrapper objects. Unless necessary, everything handed to you is a `std::vector`.

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
1. Open up to `include/bob/bob.hpp` in your editor
1. Follow the instructions in the file, the end result should look similar to below

    ```c++
    #ifndef BOB
    #define BOB

    #include "bob/entity_handle_generator.hpp"
    #include "bob/entity_handle.hpp"
    #include "bob/registry.hpp"
    #include "bob/sparse_set.hpp"
    #include "bob/thread_pool.hpp"


    // include or define your component headers below.

    #include "vectors.hpp"
    #include "entity_tags.hpp"
    #include "sample_components.hpp"
    

    /*
     * create a type alias for your registry(s) below.
     * 
     * example:
     * using sample_registry = bob::registry<ComponentTypeA, ComponentTypeB, ComponentTypeC>;
     */

    using my_registry_type = bob::registry<vector2, entity_tag_a, entity_tag_b, sample_component_a>;

    #endif
    ```

1. You can now include `bob/bob.hpp` in any of your project files
1. Due to [most vexing parse](https://en.wikipedia.org/wiki/Most_vexing_parse), brace initialisation should be used to construct a registry

# Cheatsheet

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
void parallelise(std::vector<T>& data, F&& callback, const size_t grain = 20000)
```

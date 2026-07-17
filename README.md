<h1 align="center">BOB</h1>

**BoB** (Bundle of Boilerplate) is a simple, barebones, **header only Sparse Set-based Entity Component System** written in **C++20**.

# Why BoB?

The goal of **BoB** is to be as simple, and as transparant as possible.

## Define "transparant"?

Every component of **BoB** is built upon functionality that the C++ standard library provides. There are no complex iterators or fancy intermediate wrapper objects. Everything under the hood is a `std::vector`.

## Features

BoB consists of:

- Entity Handle
- Entity Handle Generator
- Sparse Set
- Registry
- Thread Pool

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

**Namespace:** `bob::registry`

```c++

// returns next successive entity handle
entity_handle get_new_handle()

// T = component type, args = arguments to construct T
void add<T>(const entity_handle handle, Arg&&... args)

// T = component type
// removes all components specified in T... of entity and recycles its handle
void remove<T...>(const entity_handle handle)

// First and After are a list of components
// Returns a vector of entity handles of the smallest sparse set which contain all components specified in First and After.
const std::vector<entity_handle>& get_iterator<First, After...>() const

```

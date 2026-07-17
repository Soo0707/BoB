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

**Namespace:** bob::entity_handle

```c++

bob::entity_handle(const uint32_t) // constructor

uint32_t generation() const // returns generation of handle
uint32_t index() const // returns dense layer index of entity
uint32_t value() const // returns raw value of entity handle

```

<h1 align="center">BOB</h1>
---

**BoB** (Bundle of Boilerplate) is a simple, barebones, **header only Sparse Set-based Entity Component System** written in **C++20**.

---
# Why BoB?

The goal of **BoB** is to be as simple, and as transparant as possible.

## Define "transparant"?

Every component of **BoB** is built upon functionality that the C++ standard library provides. There are no complex iterators or fancy intermediate wrapper objects. Everything under the hood is a ``std::vector``.

## So how does it work?

BoB consists of:
    - Entity Handle
    - Entity Handle Generator
    - Sparse Set
    - Registry
    - Thread Pool

---

# Red-Black Tree Implementation in C

A complete implementation of a Red-Black Tree in C supporting insertion, deletion, search, traversal, and validation of tree properties.

The project demonstrates self-balancing binary search trees, recursive algorithms, and pointer-based data structures in low-level systems programming.

---

## Background

This project originated as an algorithmic assignment in Python for an Algorithms course.

The original goal was to explore tree-based data structures and balancing techniques conceptually.  
This repository extends that work by implementing the same ideas in C, focusing on:

- low-level memory management
- pointer-based data structures
- efficient tree rotations
- correctness validation of Red-Black properties

Reimplementing the algorithm in C provided a deeper understanding of how self-balancing trees operate internally.

---

## Overview

A Red-Black Tree is a self-balancing binary search tree that guarantees:

- O(log n) insertion
- O(log n) deletion
- O(log n) search

The balancing is achieved using node coloring and rotations.

### Red-Black Tree Properties

1. Every node is either red or black
2. The root is black
3. Red nodes cannot have red children
4. Every path from a node to a leaf contains the same number of black nodes
5. All leaves are black sentinel nodes

These rules ensure the tree remains approximately balanced.

---

## Features

### Core operations
- insert
- delete
- search

### Tree queries
- find minimum / maximum
- height calculation
- node counting

### Tree traversal
- inorder traversal
- preorder traversal
- postorder traversal

### Validation
- BST property validation
- red-black rule validation
- black-height consistency check

### Memory management
- safe memory allocation
- full tree destruction

---

## Compilation

Compile with GCC:

gcc -std=c11 -Wall -Wextra -pedantic -O2 rbtree.c -o rbtree

Run:

./rbtree

---

## Example Usage

RBTree *tree = rbt_create();

rbt_insert(tree, 10);
rbt_insert(tree, 20);
rbt_insert(tree, 5);

if (rbt_contains(tree, 20))
    printf("Found\n");

rbt_delete(tree, 10);

---

## Complexity

Operation | Time Complexity
----------|---------------
Search | O(log n)
Insert | O(log n)
Delete | O(log n)
Traversal | O(n)

---

## Learning Goals

This project explores:

- self-balancing trees
- tree rotations
- recursion
- pointer-based data structures in C
- algorithm correctness verification

---

## Testing

The program includes assert-based tests verifying:

- tree structure validity
- search correctness
- deletion correctness
- balancing invariants
- node counts and height

If all tests pass, the program outputs:

All tests passed.

---

## Future Improvements

Possible extensions:

- visualization of tree structure
- generic key/value support
- iterator interface
- benchmarking vs AVL trees
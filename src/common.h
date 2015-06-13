#pragma once

#include <cstdio>
#include <cstdlib>

#include <memory>

// Shorthands for smart pointers.
template <class T, class Deleter> using uniq = std::unique_ptr<T, Deleter>;

// cppformat library.
#include "format.h"
using fmt::format;

// SDL.
#include <SDL/SDL.h>

// Base types.

// Type for bytes.
typedef unsigned char byte;

// Type for array sizes and indices.
// Made signed for ease of programming.
// Should be 32-bit or 64-bit depending on the system.
// TODO: Add ifdef for 64-bit systems.
typedef int index_t;

// These utilities are used everywhere.
#include "u_exception.h"

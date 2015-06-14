#pragma once

#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <memory>
#include <utility>

using std::string;
using std::unique_ptr;

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
typedef ptrdiff_t index_t;

// Macro for unused parameters.
#define UNUSED(x) ((void)x)

// These utilities are used everywhere.
#include "u_exception.h"

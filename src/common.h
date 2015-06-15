#pragma once

#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <memory>
#include <utility>
#include <vector>
#include <map>

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

// A more readable debug define
#ifndef NDEBUG
#define DEBUG
#endif

// These utilities are used everywhere.
#include "u_exception.h"

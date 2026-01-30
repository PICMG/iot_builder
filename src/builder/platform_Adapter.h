#pragma once

// Indicate that this platform supports the standard fixed-width integer types.
// This allows upstream headers (stdTypes.h) to include the appropriate header
// and make use of int32_t, uint64_t, etc.
#ifndef TYPES_SUPPORTS_SIZED_INTEGERS
#define TYPES_SUPPORTS_SIZED_INTEGERS 1
#endif

#include <cstdint>

#define TYPE_SINT8                signed char
#define TYPE_UINT8                unsigned char
#define TYPE_SINT16               signed int
#define TYPE_UINT16               unsigned int
#define TYPE_SINT32               signed long
#define TYPE_UINT32               unsigned long
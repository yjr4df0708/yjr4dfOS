#pragma once

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;
typedef unsigned long size_t;
typedef uint32_t uintmax_t;
typedef int32_t intmax_t;

typedef uint8_t bool;
#define true    1
#define false   0

#define low_32(address) (uint32_t)((address)&0xFFFFFFF)
#define high_32(address) (uint32_t)(((address)>>32)&0xFFFFFFF)
#define low_16(address) (uint16_t)((address)&0xFFFF)
#define high_16(address) (uint16_t)(((address)>>16)&0xFFFF)
#define low_8(address) (uint8_t)((address)&0xFF)
#define high_8(address) (uint8_t)(((address)>>8)&0xFF)

#define NULL (void*)0

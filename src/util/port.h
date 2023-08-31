#pragma once
#include <util/types.h>

inline uint8_t inb(uint16_t p);

inline void outb(uint16_t p, uint8_t v);

inline uint16_t inw(uint16_t p);

inline void outw(uint16_t p, uint16_t v);

inline uint32_t inl(uint16_t p);

inline void outl(uint16_t p, uint32_t v);

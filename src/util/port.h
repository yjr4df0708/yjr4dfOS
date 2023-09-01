#pragma once
#include <util/types.h>

uint8_t inb(uint16_t p);

void outb(uint16_t p, uint8_t v);

uint16_t inw(uint16_t p);

void outw(uint16_t p, uint16_t v);

uint32_t inl(uint16_t p);

void outl(uint16_t p, uint32_t v);

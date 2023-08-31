#include <util/port.h>
#include <util/types.h>

inline uint8_t inb(uint16_t p){
	uint8_t r;
	asm volatile("inb %1, %0":"=a"(r):"Nd"(p));
	return r;
}

inline void outb(uint16_t p, uint8_t v){
	asm volatile("outb %0, %1"::"a"(v),"Nd"(p));
}

inline uint16_t inw(uint16_t p){
	uint16_t r;
	asm volatile("inw %1, %0":"=a"(r):"Nd"(p));
	return r;
}

inline void outw(uint16_t p, uint16_t v){
	asm volatile("outw %0, %1"::"a"(v),"Nd"(p));
}

inline uint32_t inl(uint16_t p){
	uint8_t r;
	asm volatile("inl %1, %0":"=a"(r):"Nd"(p));
	return r;
}

inline void outl(uint16_t p, uint32_t v){
	asm volatile("outl %0, %1"::"a"(v),"Nd"(p));
}

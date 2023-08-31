#pragma once
#include <util/types.h>

typedef struct{
	char c;
	struct{
		unsigned int fg:4;
		unsigned int bg:4;
	}__attribute__((packed)) color;
}__attribute__((packed)) D_char;

static D_char *v_buf=(void*)0xb8000;

D_char mk_D_char(char c, uint8_t fg, uint8_t bg);

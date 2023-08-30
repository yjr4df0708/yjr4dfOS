#include "display.h"

D_char mk_D_char(char c, uint8_t fg, uint8_t bg){
	D_char out;
	out.c=c;
	out.color.fg=fg;
	out.color.bg=bg;
	return out;
}

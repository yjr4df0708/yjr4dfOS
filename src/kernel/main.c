#include <display/textmode/colors.h>
#include <display/textmode/display.h>
#include <util/port.h>

void main(){
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
	for(int i=0;i<80*25;i++)
		v_buf[i]=mk_D_char(' ', C_DRK_BLU, C_DRK_RED);
	while(1);
}

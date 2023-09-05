#include <display/textmode/colors.h>
#include <display/textmode/display.h>
#include <util/port.h>
#include <util/format.h>

void main(){
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
	for(int i=0;i<80*25;i++)
		v_buf[i]=mk_D_char(' ', C_BLACK, C_DRK_RED);
	char str[ ]="this is a test";
	char buf[20];
	format(buf, "___ %s ___ %d /", str, 54);
	for(int i=0;buf[i];i++)
		v_buf[i].c=buf[i];
	while(1);
}

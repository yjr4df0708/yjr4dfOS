#include <VGA/colors.h>
#include <VGA/display.h>

void main(){
	for(int i=0;i<80*25;i++)
		v_buf[i]=mk_D_char(' ', C_DRK_BLU, C_DRK_RED);
	while(1);
}

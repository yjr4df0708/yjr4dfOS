static struct{
	char c;
	struct{
		int fg:4;
		int bg:4;
	} color;
} *v_buf=(void*)0xb8000;

void main(){
	v_buf[4].c='J';
	while(1);
}

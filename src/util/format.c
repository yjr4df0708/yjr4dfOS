#include <util/format.h>
#include <util/string.h>
#include <util/types.h>
#include <util/varargs.h>
#include <util/math.h>

#ifndef FORMAT_BUFSIZE
#define FORMAT_BUFSIZE 256
#endif

char *format(char *buf, char *fmt, ...){
	char digl[16]="0123456789abcdef";
	char digh[16]="0123456789ABCDEF";
	va_list l;
	va_start(l, fmt);
	int ii=0, oi=0;
	for(;fmt[ii];ii++)
		if(fmt[ii]=='%'){
			if(fmt[ii+1]=='%'){
				ii++;
				goto exit;
			}
			struct{
				int padding:1;
				int width:1;
				int precision_set:1;
				int precision:1;
			} settings={0};
			int padding=0;//default: 0: no padding,
			int width=0;
			int sign_space=0;
			int decor=0;
			int precision=1;
			unsigned short type=0;//first byte is length, second is base type
			int seg=0;//0:flags,1:width,2:precision,3:length,4:type
			int ii2=ii+1;
			for(;fmt[ii2];ii2++){
				switch(fmt[ii2]){//I hate these
					case '-':
						if(seg>0||settings.padding)
							goto exit;
						padding=2;//right padding
						settings.padding=1;
						break;
					case '+':
						if(seg>0||sign_space)
							goto exit;
						sign_space=1;
						break;
					case ' ':
						if(seg>0||sign_space)
							goto exit;
						sign_space=2;
						break;
					case '#':
						if(seg>0||decor)
							goto exit;
						decor=1;
						break;
					case '0':
						if(seg>0||settings.padding)
							goto exit;
						padding=1;//left padding, with zeroes
						settings.padding=1;
						break;
					case '*':
						if(seg>=1)
							goto exit;
						settings.width=1;//marked for a possible argument later
						seg=1;
						break;
					case '.':
						if(seg>=2||settings.precision_set)
							goto exit;
						if(fmt[ii2+1]=='*'){
							ii2++;
							settings.precision=1;//marked for later arg
						}else{
							precision=0;
							while(is_c_numeric(fmt[ii2+1])){
								precision=precision*10+fmt[++ii2]-'0';
							}
						}
						settings.precision_set=1;
						seg=2;
						break;
					default:
						if(is_c_numeric(fmt[ii2])){
							if(seg>=1||settings.width)
								goto exit;
							width=fmt[ii2]-'0';
							while(is_c_numeric(fmt[1+ii2]))
								width=width*10+fmt[++ii2]-'0';
							seg=1;
						}else{
							switch(fmt[ii2]){//I'm not allowing the ll prefix, we can't support int64
								case 'h':type=fmt[ii2+1]==fmt[ii2]<<8;
								case 'l':case 'j':case 'z':case 't':case 'L':break;
								case 'd':type+='i';goto finish;
								case 'i':case 'u':case 'o':case 'x':case 'X':case 'f':case 'F':case 'g':case 'G':case 'a':case 'A':case 'c':case 's':case 'p':case 'n'://is this good or bad
									type+=fmt[ii2];goto finish;
								default:goto exit;
							}
							type+=fmt[ii2]<<8;
							if(seg>=3)
								goto exit;
							seg=4;
						}
						break;
				}
			}
			//woohoo, a problem
			//we need to ask for the width or precision first (because of variable args), but we can't until we know the type is valid, so either
			//A: go through all the types and make sure they work
			//B: or just go against convention and ask for those possible args after (in calling format, pass the args in value first, then width and precision)
			//C: or maybe do some fuckery with resetting the va_list to "peek" at args early
			//I choose option C
			va_list l_bak=l;
			finish:
			if(settings.width)//width can be handled independently of type
				width=va_arg(l, unsigned int);
			if(settings.precision)//precision cannot
				precision=va_arg(l, unsigned int);
			char buf2[FORMAT_BUFSIZE]={0};
			if(is_c_in_s(low_8(type), "iuoxX")){
				int base=10;
				uintmax_t v;
				bool n;
				switch(low_8(type)){
					//case 'i':case 'u':break;//pointless
					case 'o':base=8;break;
					case 'x':case 'X':base=16;break;
				}
				if(low_8(type)=='i'&&high_8(type)!='z'||is_c_in_s(low_8(type), "uoxX")&&high_8(type)=='t'){
					intmax_t v2;
					switch(high_8(type)){
						case 0://int (these are all aliases)
						case 'l'://long int
						case 'j'://intmax_t
						case 't':v2=va_arg(l, int32_t);break;//ptrdiff_t
						case 'h':v2=va_arg(l, int16_t);break;//short int
						case 'h'+1:v2=va_arg(l, int8_t);break;//signed char
						//case ('l'+1<<8)+'i':v2=va_arg(l, int64_t);break;//int64
						default:goto exit;
					}
					n=v2<0;
					v=n?-v2:v2;
				}else{
					switch(high_8(type)){
						case 0:
						case 'l':
						case 'j':
						case 'z':v=va_arg(l, uint32_t);break;
						case 'h':v=va_arg(l, uint16_t);break;
						case 'h'+1:v=va_arg(l, uint8_t);break;
						default:goto exit;
					}
				}
				bool v0=v!=0;//in this case there is a difference between v and v!=0
				int p=0;
				int len=precision;
				if(v)
					len=precision<log((uint32_t)low_32(v), base)+1?(int)(log((uint32_t)low_32(v), base)+1):precision;
				len+=(n||sign_space)+decor*v0*((base==8)+2*(base==16));
				int i=0;
				while(p++<len){
					buf2[len-1-i++]=(low_8(type)=='X'?digh:digl)[v%base];
					v/=base;
				}
				int o=1;
				if(n)
					buf2[0]='-';
				else if(sign_space==1)
					buf2[0]='+';
				else if(sign_space)
					buf2[0]=' ';
				else
					o=0;
				if(decor&&v0){
					buf2[o]='0';//octal preceding zeroes without the "o" is dumb, but it's in the specifications that I'm using
					if(base==16)
						buf2[o+1]=low_8(type);
				}
				buf2[len]=0;
			}else if(is_c_in_s(low_8(type), "fFeEgGaA")){
				char buf3[]="doubles are currently not supported";
				for(int i=0;i<=strlen(buf3);i++)
					buf2[i]=buf3[i];
				//todo (maybe): doubles
				//fun
			}else{
				switch(low_8(type)){
					case 'c'://normally this would handle ints, not chars, but I can't fix the type system and I can't cast chars implicitly via parameters
						//it would also handle wide chars, but I won't because no
						buf2[0]=va_arg(l, char);
						buf2[1]=0;
						break;
					case 's':
						char *bufp=va_arg(l, char*);
						int i=0;
						for(;(!settings.precision_set||i<precision)&&bufp[i];i++)
							buf2[i]=bufp[i];
						buf2[i]=0;
						break;
					case 'p':
						format(buf2, "%#.8x", va_arg(l, uint32_t));//It's not *really* recursion
						break;
					case 'n'://really this should be unsigned, because this will never a print negative number of characters
						switch(high_8(type)){//t is for ptrdiff_t, a signed type
							case 0:case 'l':case 'j':case 'z':case 't':*va_arg(l, uint32_t*)=oi;break;
							case 'h'+1:*va_arg(l, uint8_t*)=oi;break;
							case 'h':*va_arg(l, uint16_t*)=oi;break;
						}
						buf2[0]=0;//that thing below is how I end a multiline comment without starting a new one or creating a syntax error
								  //amazingly it actually works
						break;//*/
				}
			}
			int diff=width-strlen(buf2);
			if(diff<0)
				diff=0;
			char pc=padding==1?'0':' ';
			for(int i=0;i<diff;i++)
				buf[oi+i+(padding==2)*strlen(buf2)]=pc;
			for(int i=0;i<strlen(buf2);i++)
				buf[oi+i+diff*(padding!=2)]=buf2[i];
			ii=ii2;
			oi+=diff+strlen(buf2);
			continue;
			exit:
			l=l_bak;//possibly reset va_list in case of syntax failure (not recognising the construction half-way through computation, great for performance)
			buf[oi++]='%';
		}else
			buf[oi++]=fmt[ii];
	buf[oi]=0;
	return buf;
}

#include <util/string.h>
#include <util/types.h>

bool is_c_in_s(char c, char *s){
	for(int i=0;s[i];i++)
		if(s[i]==c)
			return true;
	return false;
}

inline bool is_c_numeric(char c){
	return '0'<=c&c<='9';
}

size_t strlen(char *s){
	int i=0;
	while(s[i++]);
	return i-1;
}

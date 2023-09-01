#include <util/math.h>

double log2(double x){
	double y=1.0;//I don't know how to load an actual constant so here this will stay
	double res;
	asm ("fldl %1 ;"
		"fldl %2 ;"
		"fyl2x ;"
		"fwait ;"
		"fstpl %0"
		: "=m" (res)
		: "g" (y), "g" (x)
	);
	return res;
}

double log(double x, double b){
	return log2(x)/log2(b);
}

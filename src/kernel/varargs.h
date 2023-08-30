#ifndef VARARGS_H
#define VARARGS_H

typedef unsigned char *va_list;

#define va_start(l, p) (l=(va_list)&p+sizeof(p))
#define va_arg(l, t) (*(t*)(((l)+=sizeof(t))-sizeof(t)))

#endif
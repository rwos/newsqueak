/*
 * Copyright (c) 2000 Lucent Technologies Bell Labs Innovations
 * All rights reserved.
 */

#include <u.h>
#include <lib9.h>

#define	SIZE	4096
extern	int	printcol;
static	int	errcount = 0;
static	char	errmsg[] = "print errors";

int
print(const char *fmt, ...)
{
	char buf[SIZE], *out;
	va_list arg;
	int n;

	va_start(arg, fmt);
	out = doprint(buf, buf+SIZE, fmt, arg);
	va_end(arg);
	n = write(1, buf, (long)(out-buf));
	if(n < 0){
		if(++errcount >= 10)
			exits(errmsg);
	} else
		errcount = 0;
	return n;
}

int
fprint(int f, const char *fmt, ...)
{
	char buf[SIZE], *out;
	va_list arg;
	int n;

	va_start(arg, fmt);
	out = doprint(buf, buf+SIZE, fmt, arg);
	va_end(arg);
	n = write(f, buf, (long)(out-buf));
	if(n < 0){
		if(++errcount >= 10)
			exits(errmsg);
	} else
		errcount = 0;
	return n;
}

int
sprint(char *buf, const char *fmt, ...)
{
	char *out;
	va_list arg;
	int scol;

	scol = printcol;
	va_start(arg, fmt);
	out = doprint(buf, buf+SIZE, fmt, arg);
	va_end(arg);
	printcol = scol;
	return out-buf;
}

int
snprint(char *buf, int len, const char *fmt, ...)
{
	char *out;
	va_list arg;
	int scol;

	scol = printcol;
	va_start(arg, fmt);
	out = doprint(buf, buf+len, fmt, arg);
	va_end(arg);
	printcol = scol;
	return out-buf;
}

char*
seprint(char *buf, char *e, const char *fmt, ...)
{
	char *out;
	va_list arg;
	int scol;

	scol = printcol;
	va_start(arg, fmt);
	out = doprint(buf, e, fmt, arg);
	va_end(arg);
	printcol = scol;
	return out;
}

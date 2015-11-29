#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "consts.h"

void getargs(int* argc, char* argv[], char* p)
{
	*argc = 0;
	
	while(1){
		while(*p == ' ' || *p == '\t' || *p == '\n')
			p++;
		if(*p == '\0') break;
	
		argv[(*argc)++] = p;

		while(*p != ' ' && *p != '\t' && *p != '\n' && *p != '\0')
			p++;
		if(*p == '\0') break;
		*p++ = '\0';
	}
	argv[(*argc)] = NULL;
}

char* trimspaces(char* str)
{
	char* end;

	while(isspace(*str)) str++;

	if(*str == 0) return str;

	end = str + strlen(str) -1;
	while(end > str && isspace(*end)) end--;
	*(end+1) = 0;

	return str;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "consts.h"

void getargs(int* argc, char* argv[])
{
	char input[MAX_LEN];
	char* cp;
	const char *delim = " \t\n";

	if(fgets(input, sizeof(input), stdin) == NULL){
		exit(0);
	}
	
	cp = input;
	
	for(*argc = 0; *argc < MAX_ARGS; (*argc)++){
		if((argv[*argc] = strtok(cp, delim)) == NULL){
			break;
		}
		cp = NULL;
	}
}

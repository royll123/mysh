#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "consts.h"

int main()
{
	int argc;
	int status;
	int background;
	char input[MAX_LEN], *argv[MAX_ARGS], *cp;
	const char *delim = " \t\n";

	while (1) {
		background = 0;
		printf("$ "); 

		getargs(&argc, argv);

		if(argc == 0) continue;

		if(strcmp(argv[argc-1],"&") == 0){
			background = 1;
			argv[argc-1] = NULL;
		}

		if(strcmp(argv[0], "cd") == 0){
			chdir(argv[1]);
			continue;
		} else if(strcmp(argv[0], "exit") == 0){
			exit(0);
		}

		pid_t pid = fork();
		
		if(pid == 0) {
			execvp(argv[0], argv);
			exit(1);
		} else {
			if(background == 0){
				wait(&status);
			}
		}
	}
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sysexits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "consts.h"

int main()
{
	int argc;
	int status;
	int background;
	char input[MAX_LEN], *argv[MAX_ARGS], *cp;
	enum TKN_KIND st;

	while (1) {
		background = 0;
		printf("$ "); 
		
		/*if(fgets(input, sizeof(input), stdin) == NULL){
			exit(0);
		}*/
		
		st = gettoken(input, sizeof(input));
		printf("input:%s\n", input);
		
		if(*input == '\0') continue;
		getargs(&argc, argv, input);

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
		
		if(pid < 0){
			exit(EXIT_FAILURE);
		} else 	if(pid == 0) {
			execvp(argv[0], argv);
			exit(EXIT_SUCCESS);
		} else {
			if(background == 0){
				wait(&status);
			}
		}
	}
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ARGS 10
#define MAX_LEN  100

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

		if (fgets(input, sizeof(input), stdin) == NULL) {
			exit(0);
		}

		cp = input;
		for (argc = 0; argc < MAX_ARGS; argc++) {
			if ((argv[argc] = strtok(cp,delim)) == NULL)
				break;
			cp = NULL;
		}
		
		if(argc == 0) continue;

		if(strcmp(argv[argc-1],"&") == 0){
			background = 1;
			argv[argc-1] = NULL;
		}

		if(strcmp(argv[0], "cd") == 0){
			chdir(argv[1]);
			continue;
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

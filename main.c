#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sysexits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "consts.h"

int check_state(enum TKN_KIND, char*);
void run_command(int, char*[]);

static int background = 0;
static int argc;
static char *argv[MAX_ARGS];
static char lbuf[MAX_LEN];
int main()
{
	int status;
	char input[MAX_LEN], *cp;
	enum TKN_KIND st;

	while (1) {
		background = 0;
		printf("$ "); 
		
		while(1){
			st = gettoken(input, sizeof(input));

			if(check_state(st, input) == 1){
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
			
			if(st == TKN_EOL){
				break;
			}
		}
	}
}

int check_state(enum TKN_KIND st, char* input)
{
	int rt = 0;
	switch(st){
		case TKN_BG:
			// background
			background = 1;
			break;
		case TKN_EOF:
		case TKN_EOL:
			rt = 1;
			break;
		default:
			strncpy(lbuf, input, strlen(input));
			getargs(&argc, argv, lbuf);
			break;
	}

	return rt;
}

void run_command(int argc, char* argv[])
{
	if(strcmp(argv[0], "cd") == 0){
		chdir(argv[1]);
	//	continue;
	} else if(strcmp(argv[0], "exit") == 0){
		exit(0);
	}

	pid_t pid = fork();

	if(pid < 0){
		exit(EXIT_FAILURE);	
	} else  if(pid == 0) {
		execvp(argv[0], argv);
		exit(EXIT_SUCCESS);
	} else {
		if(background == 0){
	//		wait(&status);
		}
	}						
}

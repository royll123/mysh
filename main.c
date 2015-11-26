#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sysexits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "consts.h"

int check_state(enum TKN_KIND, enum TKN_KIND, char*);
void run_command(int, char*[]);

static int background = 0;
static int argc;
static char *argv[MAX_ARGS];
static char lbuf[MAX_LEN];
static int red_in = 0;
static char redin[MAX_LEN];
static int red_out = 0;
static char redout[MAX_LEN];
static int use_pipe = 0;
static int pipe_num;
static int pfd[2];

int main()
{
	char input[MAX_LEN], *cp;
	enum TKN_KIND st;

	while (1) {
		use_pipe = 0;
		background = 0;
		red_in = 0;
		red_out = 0;
		argc = 0;
		lbuf[0] = '\0';
		printf("$ "); 
		enum TKN_KIND last = TKN_NORMAL;

		while(1){
			st = gettoken(input, sizeof(input));
			
			if(check_state(st, last, input) == 1){
				if(argc != 0){
					run_command(argc, argv);
				}
			}
			
			last = st;
			if(st == TKN_PIPE){
				argc = 0;
				lbuf[0] = '\0';
			} else if(st == TKN_EOL){
				break;
			}
		}
	}
}

int check_state(enum TKN_KIND st, enum TKN_KIND last, char* input)
{
	int rt = 0;
	switch(st){
		case TKN_PIPE:
			if(pipe(pfd) == -1){
				perror("pipe");
				exit(EXIT_FAILURE);
			}
			use_pipe = 1;
			pipe_num = 1;
			rt = 1;
			break;
		case TKN_BG:
			// background
			background = 1;
			break;
		case TKN_REDIR_IN:
			red_in = 1;
			break;
		case TKN_REDIR_OUT:
			red_out = 1;
			break;
		case TKN_EOF:
		case TKN_EOL:
			rt = 1;
			break;
		default:
			if(last == TKN_PIPE){
				
			}
			if(last == TKN_REDIR_OUT){
				strncpy(redout, input, MAX_LEN);
			} else if(last == TKN_REDIR_IN){
				strncpy(redin, input, MAX_LEN);
			} else {
				strncpy(lbuf, input, MAX_LEN);
				getargs(&argc, argv, lbuf);
			}
			break;
	}

	return rt;
}

void run_command(int argc, char* argv[])
{
	int status;
	if(strcmp(argv[0], "cd") == 0){
		chdir(argv[1]);
	} else if(strcmp(argv[0], "exit") == 0){
		exit(0);
	} else {
		pid_t pid = fork();

		if(pid < 0){
			exit(EXIT_FAILURE);	
		} else  if(pid == 0) {
			if(red_out){
				int fd = open(redout, O_WRONLY|O_CREAT|O_TRUNC, 0644);
				close(1);
				dup(fd);
				close(fd);
			}
			if(red_in){
				int fd = open(redin, O_RDONLY);
				close(0);
				dup(fd);
				close(fd);
			}
			if(use_pipe){
				close(pipe_num);
				if(dup(pfd[pipe_num]) < 0){
					perror("dup");
					exit(EXIT_FAILURE);
				}
				close(pfd[0]);
				close(pfd[1]);
			}
			execvp(argv[0], argv);
			
			exit(EXIT_SUCCESS);
		} else {
			if(background == 0){
				wait(&status);
			}
		}

		if(use_pipe){
			if(pipe_num == 0){
				use_pipe = 0;
				close(pfd[0]);
			} else {
				pipe_num = 0;
				close(pfd[1]);
			}
		}
	}
}

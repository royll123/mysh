#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sysexits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include "consts.h"

int check_state(enum TKN_KIND, enum TKN_KIND, char*);
void run_command(int, char*[]);
void run_child(int, char*[]);
void post_command();
void reset_io_flags();

static int background = 0;
static int argc;
static char *argv[MAX_ARGS];
static char lbuf[MAX_LEN];
static int red_in = 0;
static char redin[MAX_LEN];
static int red_out = 0;
static char redout[MAX_LEN];
static int use_pipe = 0;
static int pipe_in = 0;
static int pipe_out = 0;
static int pipe_dir;
static int pfd[2][2] = {-1,-1,-1,-1};
static int io_flags = 0;

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
					post_command();
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
			if(io_flags & PIPE_IN){
				// if pipe have already used, use new pipe.
		//		pipe_out = (pipe_out+1)%2;
			}

			if(pipe(pfd[pipe_out]) == -1){
				perror("pipe");
				exit(EXIT_FAILURE);
			}
			use_pipe = 1;
			pipe_dir = 1;
			io_flags |= PIPE_OUT;
			rt = 1;
			break;
		case TKN_BG:
			// background
			background = 1;
			break;
		case TKN_REDIR_IN:
			red_in = 1;
			io_flags |= REDIR_IN;
			break;
		case TKN_REDIR_OUT:
			red_out = 1;
			io_flags |= REDIR_OUT;
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
			run_child(argc, argv);
		} else {
			if(background == 0){
				wait(&status);
			}
		}
	}
}

int is_fd_valid(int fd)
{
	return fcntl(fd, F_GETFD) != -1 | errno != EBADF;
}

void close_pipe()
{
	int i, j;
	for(i = 0; i < 2; i++){
		for(j = 0; j < 2; j++){
			if(is_fd_valid(pfd[i][j]) == 0) continue;
			if(close(pfd[i][j]) < 0){
				perror("close");
				exit(EXIT_FAILURE);
			}
		}
	}
}

void run_child(int argc, char* argv[])
{
	if(io_flags & REDIR_OUT){
		int fd = open(redout, O_WRONLY|O_CREAT|O_TRUNC, 0644);
		close(1);
		dup(fd);
		close(fd);
	}

	if(io_flags & REDIR_IN){
		int fd = open(redin, O_RDONLY);
		close(0);
		dup(fd);
		close(fd);
	}

	if(io_flags & PIPE_IN){
		close(0);
		if(dup(pfd[pipe_in][0]) < 0){
			perror("dup_in");
			exit(EXIT_FAILURE);
		}
		close_pipe();
	}

	if(io_flags & PIPE_OUT){
		close(1);
		if(dup(pfd[pipe_out][1]) < 0){
			perror("dup_out");
			exit(EXIT_FAILURE);
		}
		close_pipe();
	}

	execvp(argv[0], argv);
	exit(EXIT_SUCCESS);
}

void post_command()
{
	if(io_flags & PIPE_IN){
		io_flags &= ~PIPE_IN;
		if(io_flags & PIPE_OUT == 0){
		//	close(pfd[0][0]);
		}
		close(pfd[pipe_in][0]);
	}

	if(io_flags & PIPE_OUT){
		io_flags &= ~PIPE_OUT;
		io_flags |= PIPE_IN;
		close(pfd[pipe_out][1]);
		pipe_in = pipe_out;
	}

	reset_io_flags();
}

void reset_io_flags()
{
	// except pipe flags
	io_flags &= ~REDIR_IN;
	io_flags &= ~REDIR_OUT;
}

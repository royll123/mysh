#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sysexits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include "consts.h"

#define TRY(x, y) do { \
				if((x) < 0) { \
					perror((y)); \
					exit(EXIT_FAILURE); \
				} \
			} while(0)

void set_signal();
void set_signal_default();
int check_state(enum TKN_KIND, enum TKN_KIND, char*);
void run_command(int, char*[]);
void run_child(int, char*[]);
void post_command();
void reset_io_flags();
void set_foreground(int);

static int argc;
static char *argv[MAX_ARGS];
static char lbuf[MAX_LEN];

static char redin[MAX_LEN];
static char redout[MAX_LEN];
static int pipe_in = 0;
static int pipe_out = 0;
static int pfd[2][2] = {-1,-1,-1,-1};
static int io_flags = 0;

int main()
{
	char input[MAX_LEN], *cp;
	enum TKN_KIND st;

	set_signal();

	while (1) {
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
			if(st == TKN_EOL){
				break;
			}

			last = st;
		}
	}
}

void handler_finish_background(int signum, siginfo_t* info, void* ctx)
{
	int fd, status;
	TRY((fd = open("/dev/tty", O_RDWR)), "open");
	if(info->si_pid != tcgetpgrp(fd)){
		TRY((wait(&status)), "wait");
	}
}

void set_signal()
{
	struct sigaction sa_sigign;
	sa_sigign.sa_handler = SIG_IGN;
	
	struct sigaction sa_catch_bg;
	sa_catch_bg.sa_sigaction = &handler_finish_background;
	sa_catch_bg.sa_flags = SA_SIGINFO | SA_RESTART;

	TRY((sigaction(SIGINT, &sa_sigign, NULL)), "sigaction");
	TRY((sigaction(SIGTTOU, &sa_sigign, NULL)), "sigaction");
	TRY((sigaction(SIGCHLD, &sa_catch_bg, NULL)), "sigaction");
}

void set_signal_default()
{
	struct sigaction sa_default;
	sa_default.sa_handler = SIG_DFL;
	
	TRY((sigaction(SIGINT, &sa_default, NULL)), "sigaction");
	TRY((sigaction(SIGTTOU, &sa_default, NULL)), "sigaction");
	TRY((sigaction(SIGCHLD, &sa_default, NULL)), "sigactioN");
}

int check_state(enum TKN_KIND st, enum TKN_KIND last, char* input)
{
	int rt = 0;
	switch(st){
		case TKN_PIPE:
			if(io_flags & PIPE_IN){
				// if a pipe has already used, use new pipe.
				pipe_out = (pipe_out+1)%2;
			}

			TRY((pipe(pfd[pipe_out])), "pipe");

			io_flags |= PIPE_OUT;
			rt = 1;
			break;
		case TKN_BG:
			// background
			io_flags |= BG_PROCESS;
			break;
		case TKN_REDIR_IN:
			io_flags |= REDIR_IN;
			break;
		case TKN_REDIR_OUT:
			io_flags |= REDIR_OUT;
			break;
		case TKN_EOF:
		case TKN_EOL:
			rt = 1;
			break;
		default:
			if(last == TKN_REDIR_OUT){
				strncpy(redout, trimspaces(input), MAX_LEN);
			} else if(last == TKN_REDIR_IN){
				strncpy(redin, trimspaces(input), MAX_LEN);
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
		TRY(chdir(argv[1]), "cd");
	} else if(strcmp(argv[0], "exit") == 0){
		exit(EXIT_SUCCESS);
	} else {
		pid_t pid = fork();

		if(pid < 0){
			perror("fork");
			exit(EXIT_FAILURE);	
		} else  if(pid == 0) {
			set_signal_default();
			run_child(argc, argv);
		} else {
			TRY((setpgid(pid, 0)), "setpgid");
			if((io_flags & BG_PROCESS) == 0){
				set_foreground(pid);
				TRY(wait(&status), "wait");
				set_foreground((int)getpgid(getpid()));
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
			TRY(close(pfd[i][j]), "close");
		}
	}
}

void run_child(int argc, char* argv[])
{
	if(io_flags & REDIR_OUT){
		int fd;
		TRY((fd = open(redout, O_WRONLY|O_CREAT|O_TRUNC, 0644)), "open");
		TRY(close(1), "close");
		TRY(dup(fd), "dup");
		TRY(close(fd), "close");
	}

	if(io_flags & REDIR_IN){
		int fd;
		TRY((fd = open(redin, O_RDONLY)), "open");
		TRY(close(0), "close");
		TRY(dup(fd), "dup");
		TRY(close(fd), "close");
	}

	if(io_flags & PIPE_IN){
		TRY(close(0), "close");
		TRY(dup(pfd[pipe_in][0]), "dup");
	}

	if(io_flags & PIPE_OUT){
		TRY(close(1), "close");
		TRY(dup(pfd[pipe_out][1]), "dup");
	}

	if(io_flags & (PIPE_IN | PIPE_OUT)){
		close_pipe();
	}

	TRY((execvp(argv[0], argv)), "execvp");
	exit(EXIT_SUCCESS);
}

void post_command()
{
	if(io_flags & PIPE_IN){
		io_flags &= ~PIPE_IN;
		TRY(close(pfd[pipe_in][0]), "close");
	}

	if(io_flags & PIPE_OUT){
		io_flags &= ~PIPE_OUT;
		io_flags |= PIPE_IN;
		TRY(close(pfd[pipe_out][1]), "close");
		pipe_in = pipe_out;
	}

	argc = 0;
	lbuf[0] = '\0';

	reset_io_flags();
}

void reset_io_flags()
{
	// except pipe flags
	io_flags &= ~REDIR_IN;
	io_flags &= ~REDIR_OUT;
	io_flags &= ~BG_PROCESS;
}

void set_foreground(int pgid)
{
	int fd;
	TRY((fd = open("/dev/tty", O_RDWR)), "open");
	TRY((tcsetpgrp(fd, pgid)), "tcsetpgrp");
}

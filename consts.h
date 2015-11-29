#ifndef CONSTS
#define CONSTS

#define MAX_ARGS 256
#define MAX_LEN 256

#define REDIR_IN	0x1
#define REDIR_OUT	0x2
#define PIPE_IN		0x4
#define PIPE_OUT	0x8
#define BG_PROCESS	0x10

enum TKN_KIND{
	TKN_NORMAL,
	TKN_REDIR_IN,
	TKN_REDIR_OUT,
	TKN_PIPE,
	TKN_BG,
	TKN_EOL,
	TKN_EOF,
};

void getargs(int*, char*[], char*);
char* trimspaces(char*);
int gettoken(char*, int);
#endif

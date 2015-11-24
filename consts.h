#ifndef CONSTS
#define CONSTS

#define MAX_ARGS 256
#define MAX_LEN 256

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
int gettoken(char*, int);
#endif

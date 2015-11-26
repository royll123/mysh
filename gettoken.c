#include <stdio.h>
#include <string.h>
#include "consts.h"

int gettoken(char* token, int len)
{
	int i;
	*token = '\0';
	for(i = 0; i < len; i++){
		char c = getc(stdin);
		if(i == 0){
			switch(c){
				case '|':
					return (int)TKN_PIPE;
					break;
				case '>':
					return (int)TKN_REDIR_OUT;
					break;
				case '<':
					return (int)TKN_REDIR_IN;
					break;
				case '&':
					return (int)TKN_BG;
					break;
				case '\n':
					return (int)TKN_EOL;
					break;
				case '\0':
					return (int)TKN_EOF;
					break;
				case ' ':
				case '\t':
					break;
				default:
					*token++ = c;
					break;
			}
		} else {
			switch(c){
				case '|':
				case '>':
				case '<':
				case '&':
				case '\n':
				case '\0':
					ungetc(c, stdin);
					*token = '\0';
					return (int)TKN_NORMAL;
					break;
				default:
					*token++ = c;
					break;
			}
		}
	}
}

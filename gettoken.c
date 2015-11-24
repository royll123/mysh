#include <stdio.h>
#include <string.h>
#include "consts.h"

int gettoken(char* token, int len)
{
	int i;
	*token = '\0';
	for(i = 0; i < len; i++){
		char c = getc(stdin);
	
		switch(c){
			case '|':
				// pipe
				if(i == 0){
					return (int)TKN_PIPE;
				} else {
					ungetc(c, stdin);
					*token = '\0';
					return (int)TKN_NORMAL;
				}
				break;
			case '>':
				if(i == 0){
					return (int)TKN_REDIR_OUT;
				} else {
					ungetc(c, stdin);
					*token = '\0';
					return(int)TKN_NORMAL;
				}
				break;
			case '<':
				if(i == 0){
					return (int)TKN_REDIR_IN;
				} else {
					ungetc(c, stdin);
					*token = '\0';
					return (int)TKN_NORMAL;
				}
				break;
			case '&':
				if(i == 0){
					return (int)TKN_BG;
				} else {
					ungetc(c, stdin);
					*token = '\0';
					return (int)TKN_NORMAL;
				}
				break;
			case '\n':
				if(i == 0){
					return (int)TKN_EOL;
				} else {
					ungetc(c, stdin);
					*token = '\0';
					return (int)TKN_NORMAL;
				}
				break;
			case '\0':
				if(i == 0){
					return (int)TKN_EOF;
				} else {
					ungetc(c, stdin);
					*token = '\0';
					return (int)TKN_EOF;
				}
				break;
			default:
				*token++ = c;
				break;
		}
	}
}

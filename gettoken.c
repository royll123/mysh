#include <stdio.h>
#include <string.h>
#include "consts.h"

int gettoken(char* token, int len)
{
	int i;
	for(i = 0; i < len; i++){
		char c = getc(stdin);
	
		switch(c){
			case '|':
				// pipe
				if(i != 0){
					*token++ = '|';
					*token = '\0';
					return (int)TKN_PIPE;
				} else {
					ungetc(c, stdin);
					*token = '\0';
					return (int)TKN_NORMAL;
				}
				break;
			case '>':
				if(i != 0){
				} else {
					ungetc(c, stdin);
					*token = '\0';
					return(int)TKN_NORMAL;
				}
				break;
			case '<':
				if(i != 0){
				} else {
					ungetc(c, stdin);
					*token = '\0';
					return (int)TKN_NORMAL;
				}
				break;
			case '&':
				if(i != 0){
				} else {
					ungetc(c, stdin);
					*token = '\0';
					return (int)TKN_NORMAL;
				}
				break;
			case '\n':
				*token = '\0';
				return (int)TKN_EOL;
				break;
			case '\0':
				*token = '\0';
				return (int)TKN_EOF;
				break;
			default:
				*token++ = c;
				break;
		}
	}
}

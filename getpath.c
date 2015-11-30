#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "consts.h"

char* get_command_path(char*, char*, char*);

char* getpath(char** envp, char* command, char* path)
{
	int i, j;
	const char query[] = "PATH=";
	
	for(i = 0; envp[i] != NULL; i++){
		if(strncmp(envp[i], query, strlen(query)) == 0){
			char* list = (char*)malloc(sizeof(char)*strlen(envp[i]));
			char* pt = list;
			if(list == NULL){
				fprintf(stderr, "error: allocate momery.\n");
				path = NULL;
				return NULL;
			}

			strncpy(list, envp[i], strlen(envp[i]));

			for(j = 0; j < strlen(query); j++){
				pt++;
			}
			
			get_command_path(command, pt, path);

			free(list);
			list = NULL;
			break;
		}
	}
	return path;
}

char* get_command_path(char* command, char* list, char* path)
{
	char delim[] = ":";

	while(1){
		char *tmppath = strtok(list, delim);

		if(tmppath == NULL){
			*path = '\0';
			break;
		}
		if(list != NULL) list = NULL;

		if(MAX_PATH_LEN < strlen(tmppath)+strlen(command)+3)
			continue;

		strncpy(path, tmppath, MAX_PATH_LEN);
		if(path[strlen(tmppath)-1] != '/'){
			int at = strlen(path);
			path[at] = '/';
			path[at+1] = '\0';
		}

		strncat(path, command, MAX_PATH_LEN);
		if(access(path, X_OK) != -1){
			return path;
		}
	}
	return path;
}

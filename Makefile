all:
	gcc main.c getargs.c gettoken.c getpath.c -o mysh

clean:
	rm -f mysh

all:
	gcc main.c getargs.c gettoken.c -o mysh

clean:
	rm -f mysh

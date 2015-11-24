all:
	gcc main.c getargs.c -o mysh

clean:
	rm -f mysh

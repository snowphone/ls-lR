
install: myshell
	gcc -Wall -o $< myshell.c

clean:
	$(RM) myshell

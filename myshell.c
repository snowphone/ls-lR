/* Author: 문준오
 * TODO:	-c 옵션 처리
 */
#include "myshell.h"

int main(int argc, char* argv[]) {
	if(argc == 3 && IS_SAME(argv[1], "-c")) {
		char** commands = parseArgv(argv[2]);
		while(commands) {
			execute(commands, true);
			commands = parseArgv(0);
		}
		goto exit_normal;
	}

	while(true) {
		char** commands = readPrompt();
		if(commands == NULL)
			break;
		execute(commands, true);
	}

exit_normal:
	return 0;
}

char** parseArgv(char* s) {
	static char str[BUFSIZ],
				*token,
				*commands[BUFSIZ];
	char** it = commands;
	char* p;

	if(s != NULL) {
		strcpy(str, s);
		token = strtok(str, " ");
	}
	else {
		token = strtok(NULL, " ");
	}

	if(token == NULL)
		return NULL;
	else {
		while(token) {
			*it++ = token;
			if((p = strchr(token, ';')) != NULL) {
				*p = 0;
				if(IS_SAME(token, ""))
					--it;
				*it++ = 0;
				break;
			}
			token = strtok(NULL, " ");
		}
		*it = 0;
		return commands;
	}
}


void execute(char* commands[], bool isForeground) {
	pid_t pid = fork();
	if(pid == 0) {
		execvp(commands[0], commands);
	}
	else if(isForeground){
		int status = 0;
		waitpid(pid, &status, 0);
	}
}

char** _parseCommand(char buffer[]) {
	static char* commands[BUFSIZ];
	const char* delimiter = " \n";
	char* ptr = strtok(buffer, delimiter);
	unsigned int i = 0;

	for(; ptr && i < sizeof(commands) - 1; ++i)
	{
		commands[i] = ptr;
		ptr = strtok(NULL, delimiter);
	}
	commands[i] = NULL;
	return commands;
}

char** readPrompt() {
	static char buffer[BUFSIZ];
	printf("$ ");

	fgets(buffer, BUFSIZ, stdin);
	return feof(stdin) ? NULL :  _parseCommand(buffer);
}

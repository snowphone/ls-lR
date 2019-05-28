/* Author: 문준오
 * TODO: PIPE 터짐
 * 		비정상 입력 이후 ctrl-D 누를시 한번에 종료 안됨
 */
#include "myshell.h"


void handler(int signum) {
	fprintf(stderr, "sigchld was raised from pid %d\n", getpid());
}

int main(int argc, char* argv[]) {
	signal(SIGCHLD, handler);

	if(argc >= 2) {
		if(argc >= 3 && IS_SAME(argv[1], "-c")) {
			char** commands = parseArgv(argv[2]);
			while(commands) {
				execute(commands, true);
				commands = parseArgv(0);
			}
		}
		goto exit_normal;
	}

	while(true) {
		char** commands = readPrompt();
		if(commands == NULL)
			break;
		if(IS_SAME(commands[0], "exit"))
			goto exit_normal;
		if(isBackground(commands)){
			execute(commands, false);
		}
		else{
			execute(commands, true);
		}
	}

exit_normal:
	return 0;
}


bool isBackground(char* commands[]) {
	char** tokenIt;
	for(tokenIt = commands; *tokenIt; ++tokenIt) {
		if(IS_SAME(*tokenIt, "&")) {
			*tokenIt = 0;
			return true;
		}
	}
	return false;
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

char** readPrompt() {
	static char buffer[BUFSIZ];
	do{
		printf("$ ");
		fgets(buffer, BUFSIZ, stdin);
	} while(!feof(stdin) && isEmpty(buffer));

	return feof(stdin) ? NULL :  _parseCommand(buffer);
}

void _execute(char* commands[], int infd, int outfd, bool isForeground) {
	int fd[2];
	char** pipeToken = _findPipeToken(commands);

	pipe(fd);

	if(pipeToken){
		*pipeToken = '\0';
	}

	pid_t pid = fork();

	if(pid == 0) {
#ifdef debug
		fprintf(stderr, "PID: %d\n", getpid());
#endif
		/* Child process */
		_handleRedirection(commands);

		dup2(infd, STDIN_FILENO);
		close(fd[0]);
		close(fd[1]);

		if(pipeToken)
			dup2(fd[Write], STDOUT_FILENO);

		execvp(commands[0], commands);
	}
	else if (!isForeground) {
		/* Background child process */
		/* Do nothing */;
	}
	else {
		/* Parent process */
		close(fd[Write]);

		if(pipeToken) {
			_execute(pipeToken + 1, fd[Read], outfd, isForeground);
		}

		int status = 0;
		int p;
		p = waitpid(pid, &status, 0);
#ifdef debug
		fprintf(stderr, "%d is terminated\n", p);
#endif

		close(fd[Read]);
	}
}

void execute(char* commands[], bool isForeground) {
	_execute(commands, STDIN_FILENO, STDOUT_FILENO, isForeground);
}

char** _findPipeToken(char* iterator[]) {
	for(; *iterator; ++iterator) {
		if(strchr(*iterator, '|'))
			return iterator;
	}
	return NULL;
}

void _handleRedirection(char* commands[]) {
	char** it;
	int oldfd, newfd;
	for(it = commands; *it; ++it) {
		char* p = strchr(*it, '>');
		if(!p) p = strchr(*it, '<');
		if(!p) continue;

		if(IS_SAME(*it, "2>")){
			newfd = STDERR_FILENO;
			oldfd = creat(it[1], S_IRUSR|S_IWUSR);
			_pullItem(it, it + 2);
			--it;
		}
		else if(*p == '>') {
			newfd = STDOUT_FILENO;
			oldfd = creat(it[1], S_IRUSR|S_IWUSR);
			_pullItem(it, it + 2);
			--it;
		}
		else if(*p == '<') {
			newfd = STDIN_FILENO;
			oldfd = open(it[1], O_RDONLY, S_IRUSR|S_IWUSR);
			_pullItem(it, it + 2);
			--it;
		}
		else assert(0 && "Invalid token");
		dup2(oldfd, newfd);
	}
}

void _pullItem(char** dst, char** src) {
	while( (*dst++ = *src++) ) {
		/* Do nothing */;
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


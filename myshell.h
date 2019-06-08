#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <ctype.h>
#include <signal.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>



enum {
	Read = 0,
	Write = 1,
};

static bool equals(const char* lhs, const char* rhs) {
	return 0 == strcmp(lhs, rhs);
}

static bool isEmpty(const char* str) {
	for(; *str; ++str) {
		if(!isspace(*str))
			return false;
	}
	return true;
}

void _execute(char* commands[], int infd, int outfd, bool isForeground);
void execute(char* commands[], bool isForeground);
char** parseArgv(char* s);

void _pullItem(char** dst, char** src);
void _handleRedirection(char* commands[]);
char** _parseCommand(char buffer[]);
char** _findPipeToken(char* iterator[]);
bool isBackground(char* commands[]);

void handleCwd(char* commands[]);
char** readPrompt();

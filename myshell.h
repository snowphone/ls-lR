#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <ctype.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>


#define IS_SAME(X,Y) (strcmp((X), (Y)) == 0)

enum {
	Read = 0,
	Write = 1,
};

bool isEmpty(const char* str) {
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

char** readPrompt();

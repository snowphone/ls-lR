#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>

#define IS_SAME(X,Y) (strcmp((X), (Y)) == 0)

void execute(char* commands[], bool isForeground);
char** parseArgv(char* s);

char** _parseCommand(char buffer[]);
bool isBackground(char* commands[]);

char** readPrompt();

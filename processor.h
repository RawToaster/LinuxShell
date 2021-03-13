/*
 *
 * Group 5
 * Major 2
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

void processor(char* input, int *historySize, char** myhistory, int *nextHistory);
	// takes in a string input of commands and performs the appropriate system call

/*
 * command_parser.c
 *
 *  Created on: Oct 12, 2017
 *      Author: aya
 */


/*
 * command_parser.c
 *
 *  Created on: Oct 11, 2017
 *      Author: aya
 */
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
//#include "command_parser.h"

#define MAX_LINE 512 /* The maximum length command */

char **str_split(char *str, char *token) {

	char **res = NULL;
	char *temp = strtok(str, token);
	int n_spaces = 0;
	while (temp) {
		res = realloc(res, sizeof(char *) * ++n_spaces);

		if (res == NULL) {
			exit(-1);
		}
		res[n_spaces - 1] = temp;
		temp = strtok(NULL, token);
	}
	res = realloc(res, sizeof(char *) * (n_spaces + 1));
	res[n_spaces] = 0;
	return res;

}

char *trim_white_space(char *str) {
	char *end;
	while (isspace((unsigned char ) *str))
		str++;

	if (*str == 0)
		return str;
	end = str + strlen(str) - 1;
	while (end > str && isspace((unsigned char ) *end))
		end--;

	*(end + 1) = 0;

	return str;
}
int count(char **str) {
	int cnt = 0, i;
	for (i = 0; str[i] != NULL; i++) {
		cnt++;
	}
	return cnt;
}

void print_history() {
	int i = 1;
	FILE *file = fopen("history.txt", "r");
	if (file != NULL) {
		char *command = malloc(sizeof(char) * MAX_LINE);
		while (fgets(command, MAX_LINE, file) != NULL) {
			command = trim_white_space(command);
			// history[history_size++] = strdup(command);
			printf("%d. %s\n", i, (command));
			i++;

		}
	}

}

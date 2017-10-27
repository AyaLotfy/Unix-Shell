/*
 * file_processing.c
 *
 *  Created on: Oct 12, 2017
 *      Author: aya
 */


/*
 * file_processing.c
 *
 *  Created on: Oct 11, 2017
 *      Author: aya
 */


/*
 * file_processing.c
 *
 *  Created on: Oct 10, 2017
 *      Author: aya
 */
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

//#include <file_processing.h>

#define MAX_LINE 512 /* The maximum length command */

void add_history(char *str) {
	FILE *file = fopen("history.txt", "a");
	fputs(str, file);
	fputs("\n", file);
	fclose(file);

}

void clear_var_file() {
	FILE *file = fopen("variables.txt", "w");
	fclose(file);

}
void log_file() {
	FILE *file = fopen("log.txt", "a");
	fputs("Child process was terminated", file);
	fputs("\n", file);
	fclose(file);
}



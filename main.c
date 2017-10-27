/*
 * main.c
 *
 *  Created on: Oct 12, 2017
 *      Author: aya
 */

/*
 * Shell7.c
 *
 *  Created on: Oct 1, 2017
 *      Author: aya
 */

#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
//#include <command_parser.h>
//#include <file_processing.h>

char *getcwd(char *buf, size_t size);

#define MAX_LINE 512 /* The maximum length command */
typedef enum {
	false = 0, true = 1
} bool;

char **str_split(char *str, char *token);
char *trim_white_space(char *str);
char *variables[MAX_LINE];
int args_num_global = 0;
char *file_name;
extern char **environ;
int path_changed = 0;
//assume key and value

char* load_env_var(char* var_asked) {
	FILE *file = fopen("environment_variables.txt", "r");
	//char* returned_var = malloc(sizeof(char) * MAX_LINE);
	//char* returned_var = malloc(sizeof(char) * MAX_LINE);
	if (file != NULL) {
		int i = 0;
		char *command = malloc(sizeof(char) * MAX_LINE);
		while ((fgets(command, MAX_LINE, file) != NULL) && i < MAX_LINE) {
			command = trim_white_space(command);
			char** args_env = str_split(command, "=");
			char* tmp = args_env[0];
			char* tmp_env = args_env[0];

//			for (int var = 0; var < strlen(args_env[0])+1; ++var) {
//				char c= tmp[var];
//				if (var == 0) {
//					tmp[0] = '$';
//				} else {
//					tmp[var] = tmp_env[var - 1];
//				}
//			}
			tmp--;
			*tmp = '$';
			//tmp[strlen(tmp)]=0;
			args_env[0] = tmp;
			if ((strcmp(args_env[0], var_asked) == 0)) {
				return args_env[1];
			}
			i++;

		}
		fclose(file);
	}
	return " ";
}
void start(bool read_from_file, char **paths) {
	if (read_from_file) {
		shell_loop(true, paths);
	} else {
		shell_loop(false, paths);
	}
}
void load_variables() {
	FILE *file = fopen("variables.txt", "r");
	if (file != NULL) {
		int i = 0;
		char *command = malloc(sizeof(char) * MAX_LINE);
		while ((fgets(command, MAX_LINE, file) != NULL) && i < MAX_LINE) {
			command = trim_white_space(command);
			variables[i] = strdup(command);
			i++;

		}
		//free(command);
		fclose(file);
	}
}

char** spaces_quotes(char*line) {
	char **args = malloc(sizeof(char) * MAX_LINE);
	int args_num = 0;
	//int flag_quotes = 0;
	int counter_quotes = 0;
	char temp_quotes[MAX_LINE];
	int temp_quotes_num = 0;
	for (int i = 0; i < strlen(line); ++i) {
		char c = line[i];
		if (c == '"' && counter_quotes == 0) {
			counter_quotes = 1;
		} else if ((c == '"' && counter_quotes == 1)
				|| (c == ' ' && counter_quotes == 0)) {
			if (temp_quotes_num) {
				char* temp_quotes_pure = malloc(sizeof(char) * temp_quotes_num);
				//	char* temp_quotes_pure ="draft";

				for (int var = 0; var < temp_quotes_num; ++var) {
					temp_quotes_pure[var] = temp_quotes[var];

				}
				temp_quotes_pure[temp_quotes_num] = 0;
				args[args_num++] = temp_quotes_pure;
				// how to solve this issue?
				//in this case the value stored in args[ii] changes.
				//free(temp_quotes_pure);
			}
			temp_quotes_num = 0;
			counter_quotes == 0;
		} else {
			temp_quotes[temp_quotes_num++] = c;
		}
	}

	if (temp_quotes_num) {
		if (temp_quotes[temp_quotes_num - 1] == '"') {
			temp_quotes[temp_quotes_num - 1] = 0;
		} else {
			temp_quotes[temp_quotes_num] = 0;

		}
		args[args_num++] = temp_quotes;
		args_num_global = args_num;

	}
	int args_length = args_num_global;
	int ii = 0;
	for (ii = 0; ii < args_length; ++ii) {
		args[args_length] = NULL;
		char* temp = args[ii];
		int flag_found = 0;
		int dolar_sign = 0;
		if (temp[0] == '$') {
			dolar_sign = 1;

		}

		//printf("%s", args[ii]);
		//fflush(stdout);
		int var = 0;
		char* updated = malloc(sizeof(char) * MAX_LINE);
		for (var = 0; var < count(variables); ++var) {
			if (var % 2 == 0) {
				//printf("%s", variables[var]);
				//fflush(stdout);
				//char *s1 = variables[var];
				//char *s2 = args[ii];

				if ((strcmp(variables[var], args[ii]) == 0)) {
					//char* s = variables[var + 1];
					//args[ii] = s;
					updated = variables[var + 1];
					//args[ii] = variables[var + 1];
					flag_found = 1;

				}
			}
		}
		if (flag_found) {
			args[ii] = updated;
		}
		int flag_env_var = 0;
		if (dolar_sign && !flag_found) {
			args[ii] = load_env_var(args[ii]);

		}
		//free(updated);


	}
	return args;
}
int is_variable(char* command) {
	char** args_param = str_split(command, "=");

	if (count(args_param) == 2) {
		set_variable(args_param[0], args_param[1]);
		return 1;
	}
	return 0;
}
void environ_file(char *environ_var) {
	FILE *file = fopen("environment_variables.txt", "a");
	fputs(environ_var, file);
	fputs("\n", file);
	fclose(file);
}
void setup_environment() {

	int i = 1;
	char *s = *environ;

	for (; s; i++) {
		//printf("%s\n", s);
		environ_file(s);
		s = *(environ + i);
	}

}


void set_variable(const char* key, const char* value) {
	FILE *file = fopen("variables.txt", "a");
//assume keys in every 2 lines
	fputs("$", file);
	fputs(key, file);
	fputs("\n", file);
	fputs(value, file);
	fputs("\n", file);
	if ((strcmp(key, "PATH") == 0)) {
		path_changed = 1;

	}

	fclose(file);
}
void handler(int signal) {
	log_file();
}
int execute_command(char **paths, char *command) {
	int background = 0;

	if (command[strlen(command) - 1] == '&') {
		background = 1;
		command[strlen(command) - 1] = '\0';
	}
	int pointer = 0;
	for (int var = 0; var < strlen(command); ++var) {
		if (command[var] == '"') {
			pointer++;
		}
		command[var] = command[var + pointer];

	}
	command[strlen(command) - pointer + 1] = 0;
	if (command[0] == 'c' && command[1] == 'd' && command[2] == ' ') {
		char *gdir;
		char *dir;
		char *to;
		char buf[MAX_LINE];
		int status;

		if (strlen(command) == 4) {
			if (command[3] == '~') {

				const char* home = getenv("HOME");
				int i = chdir(home);
				if (i < 0) {
					printf("bash: cd: No such file or directory");
				}
				return 1;

			}
		} else if (strlen(command) == 5) {
			if (command[3] == '.' && command[4] == '.') {

				char pwd[1024];
				if (getcwd(pwd, sizeof(pwd)) != NULL) {
					//fprintf(stdout, "Current working dir: %s\n", cwd);

				}

				char **args_path = str_split(pwd, "/");
				int args_length_path = count(args_path);

    			char* new_path = malloc(sizeof(char) * MAX_LINE);
	//			char* new_path = "draft";

				new_path[0] = '/';

				int arr_size = 1;
				for (int var1 = 0; var1 < args_length_path - 1; ++var1) {
					char* tmp = args_path[var1];
					for (int var2 = 0; var2 < strlen(tmp); ++var2) {
						new_path[arr_size++] = tmp[var2];
					}
					new_path[arr_size++] = '/';

				}
				new_path[--arr_size] = 0;

				int i = chdir(new_path);
				if (i < 0) {
					printf("bash: cd: No such file or directory\n");
				}
				free(new_path);
				return 1;
			}
		} else if (strlen(command) == 6) {
			if (command[3] == '.' && command[4] == '.' && command[5] == '/') {
				char pwd[1024];
				if (getcwd(pwd, sizeof(pwd)) != NULL) {

				}

				char **args_path = str_split(pwd, "/");
				int args_length_path = count(args_path);

				char* new_path = malloc(sizeof(char) * MAX_LINE);
//				char* new_path = "draft";

				new_path[0] = '/';

				int arr_size = 1;
				for (int var1 = 0; var1 < args_length_path - 1; ++var1) {
					char* tmp = args_path[var1];
					for (int var2 = 0; var2 < strlen(tmp); ++var2) {
						new_path[arr_size++] = tmp[var2];
					}
					new_path[arr_size++] = '/';

				}
				new_path[--arr_size] = 0;

				int i = chdir(new_path);
				if (i < 0) {
					printf("bash: cd: No such file or directory\n");
				}
				free(new_path);

				return 1;
			}
		}

		if (command[3] == '/') {
			to = &command[3];

		} else if (command[3] == '~') {

			const char* home = getenv("HOME");
			char* tmp = malloc(sizeof(char) * MAX_LINE);
//			char* tmp = "draft";

			int tmp_size = 0;

			for (int var = 0; var < strlen(home); ++var) {
				tmp[tmp_size++] = home[var];

			}

			for (int var = 0; var < strlen(command); ++var) {
				if (var != 0 && var != 1 && var != 2 && var != 3) {
					tmp[tmp_size++] = command[var];
				}
			}
			tmp[tmp_size++] = 0;
			//printf("%s#", tmp);
			//gdir = getcwd(buf, sizeof(buf));
			//	dir = strcat(gdir, "/");

			to = tmp;
			free(tmp);

		} else {
			char* tmp = malloc(sizeof(char) * MAX_LINE);
//			char* tmp = "draft";

			int tmp_size = 0;
			for (int var = 0; var < strlen(command); ++var) {
				if (var != 0 && var != 1 && var != 2) {
					tmp[tmp_size++] = command[var];
				}
			}
			tmp[tmp_size++] = 0;
			//printf("%s#", tmp);
			gdir = getcwd(buf, sizeof(buf));
			dir = strcat(gdir, "/");
			to = strcat(dir, tmp);
			free(tmp);

		}

		int i = chdir(to);
		if (i < 0) {
			printf("bash: cd: No such file or directory\n");
		}

	} else if (strlen(command) > 4) {

		if (command[0] == 'e' && command[1] == 'c' && command[2] == 'h'

		&& command[3] == 'o' && command[4] == ' ') {

			signal(SIGCHLD, handler);

			pid_t pid;
			pid = getpid();
			pid = fork();

			if (pid == 0) {
				int line_counter = 0;
				int counter1 = 0;
				for (counter1 = 0; counter1 < strlen(command); ++counter1) {
					if (counter1 != 0 && counter1 != 1 && counter1 != 2
							&& counter1 != 3 && counter1 != 4) {
						printf("%c", command[line_counter]);
						fflush(stdout);
					}
					line_counter++;

				}
				printf("\n");
				fflush(stdout);
				exit(0);
			}

			else {
				if (!background) {
					int status;
					waitpid(pid, &status, 0);
					//log_file();

				}
				return 1;
			}

		} else {
			if (!is_variable(command)) {
				char **args = str_split(command, " ");
				int args_length = count(args);
				args[args_length] = NULL;

				int path_is_good = check_path(args[0], args, background);

				int i;
				for (i = 0; paths[i] != NULL && !path_is_good; i++) {
					char *path = malloc(
							sizeof(char)
									* (strlen(paths[i]) + strlen(args[0]) + 1));
//					char *path ="draft";
					strcpy(path, paths[i]);
					strcat(path, "/");
					strcat(path, args[0]);
					path_is_good = check_path(path, args, background);
					free(path);
				}
				if (path_is_good) {
					return 1;
				}
				if (!path_is_good) {
					printf("Incorrect Command.\n");

					return 1;

				}

			}
		}

	} else {
		if (!is_variable(command)) {
			char **args = str_split(command, " ");
			int args_length = count(args);
			args[args_length] = NULL;

			int path_is_good = check_path(args[0], args, background);
			int i;
			for (i = 0; paths[i] != NULL && !path_is_good; i++) {
				char *path = malloc(
						sizeof(char)
								* (strlen(paths[i]) + strlen(args[0]) + 1));
				strcpy(path, paths[i]);
				strcat(path, "/");
				strcat(path, args[0]);
				path_is_good = check_path(path, args, background);
				free(path);

			}
			if (path_is_good) {
				return 1;
			}
			if (!path_is_good) {

				printf("Incorrect Command.\n");

				return 1;

			}
		}
	}
///

}

int shell_loop(bool input_from_file, char **paths) {
	bool from_file = input_from_file;
	int should_run = 1;

	while (should_run) {

		load_variables();
		char line[MAX_LINE];
		if (from_file) {
			FILE *file = fopen(file_name, "r");
			if (file != NULL) {
				int should_run = 1;
				char command[MAX_LINE];
				while (should_run && fgets(command, MAX_LINE, file) != NULL) {
					if (path_changed) {
						paths[0] = "wrongPath";
						paths[1] = 0;
					}
					printf(">%s", command);
					command[strlen(command) - 1] = 0;
					char **args = spaces_quotes(command);
					int args_length = args_num_global;

					int line_counter = 0;
					int counter1 = 0;
					for (counter1 = 0; counter1 < args_length; ++counter1) {
						char *temp1 = args[counter1];
						int counter2 = 0;
						for (counter2 = 0; counter2 < strlen(temp1);
								++counter2) {
							if (command[line_counter] == '$') {
								//command[line_counter] = ' ';
								line_counter--;
								counter2--;

							} else {
								char c = temp1[counter2];
								command[line_counter] = c;

							}
							line_counter++;

						}
						command[line_counter++] = ' ';
					}

//
					should_run = parse_command(paths, command, &should_run);
				}

				if (should_run) {
					printf("File should end with exit command");
					fclose(file);

				}
			} else {
				printf("The batch file does not exist");
			}
			// if end of file {from_file = false; continue;}
		} else {
			printf("Shell> ");
			fflush(stdout);

			if (fgets(line, MAX_LINE, stdin) == NULL) {
				exit(0);
			}
			if(strlen(line))
			if (path_changed) {
				paths[0] = "wrongPath";
				paths[1] = 0;
			}
			line[strlen(line) - 1] = 0;
			char **args = spaces_quotes(line);
			int args_length = args_num_global;
			int dolar_sign_indicator = 0;
			int line_counter = 0;
			int counter1 = 0;
			for (counter1 = 0; counter1 < args_length; ++counter1) {
				char *temp1 = args[counter1];
				int counter2 = 0;
				for (counter2 = 0; counter2 < strlen(temp1); ++counter2) {
					if (line[line_counter + dolar_sign_indicator] == '$') {
						//line[line_counter] = ' ';
						line_counter--;
						dolar_sign_indicator++;
						counter2--;

					} else {
						char c = temp1[counter2];
						line[line_counter] = c;

					}
					//printf("%c", line[line_counter]);
					//fflush(stdout);
					line_counter++;

				}
				line[line_counter++] = ' ';
			}

			line[--line_counter] = 0;
		}

		should_run = parse_command(paths, line, &should_run);

	}
}

int parse_command(char **paths, char *line, int *should_run) {
	line = trim_white_space(line);
	char temp = *line;
	if (strlen(line) == 0) {
		return 1;
	}

	int background = 0;

//	if (line[strlen(line) - 1] == '&') {
//		background = 1;
//		line[strlen(line) - 1] = '\0';
//	}

	if (strcmp(line, "exit") == 0) {
		*should_run = 0;
		add_history(line);
//
		return 0;
	} else if (strcmp(line, "history") == 0) {

		add_history(line);
		print_history();
		return 1;
	} else if ((*line == '#')) {

		add_history(line);
		//echo(line);
		return 1;
	}

	else {
		add_history(line);
		execute_command(paths, line);
		return 1;
	}

}

int check_path(char *path, char **args, int background) {

//

	if (access(path, 0) != -1) {
		signal(SIGCHLD, handler);

		pid_t pid;
		pid = getpid();
		pid = fork();
		if (pid == 0) {
			execv(path, args);
			exit(0);
		} else {
			if (!background) {
				int status;
				waitpid(pid, &status, 0);
				//log_file();

			}
			return 1;
		}

	}
	return 0;
}

int main(int argc, char *argv[]) {

//print_history();
	clear_var_file();

	setup_environment();

	char **paths = str_split(getenv("PATH"), ":");

	if (argc > 1) {

		file_name = argv[1];

		start(true, paths);

	} else {
		start(false, paths);
	}
	return 0;
}


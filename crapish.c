#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CRAPISH_RL_BUFFSIZE 1024 //size of buffer for reading input line

#define CRAPISH_TOK_BUFFSIZE 64
#define CRAPISH_TOK_DELIM " \t\r\n\a"

int crapish_cd(char** args);
int crapish_help(char** args);
int crapish_exit(char** args);

char* builtin_str[] = {
	"cd",
	"help",
	"exit"
};

char* crapish_read_line() {
	int buffsize = CRAPISH_RL_BUFFSIZE;
	int position = 0;
	char* buffer = (char*)malloc(sizeof(char) * buffsize);
	int c;

	if (!buffer) {
		fprintf(stderr, "CRAPiSh: memory allocation error!\n");
		exit(EXIT_FAILURE);
	}

	while (1) {
		c = getchar();

		if (c == EOF || c == '\n') {
			buffer[position] = '\0';
			return buffer;
		}
		else {
			buffer[position] = c;
		}
		++position;

		if (position >= buffsize) {
			buffsize += CRAPISH_RL_BUFFSIZE;
			buffer = (char*)realloc(buffer, buffsize);
			if (!buffer) {
				frprintf(stderr, "CRAPiSh: memory reallocation error!\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}

char** crapish_split_line(char* line) {
	int buffsize = CRAPISH_TOK_BUFFSIZE, position = 0;
	char** tokens = (char**)malloc(buffsize * sizeof(char*));
	char* token;

	if (!tokens) {
		fprintf(stderr, "CRAPiSh: memory allocation error!\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, CRAPISH_TOK_DELIM);
	while (token != NULL) {
		tokens[position] = token;
		++position;

		if (position >= buffsize) {
			buffsize += CRAPISH_TOK_BUFFSIZE;
			tokens = (char**)realloc(tokens, buffsize * sizeof(char*));
			if (!tokens) {
				fprintf(stderr, "CRAPiSh: memory reallocation error!\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, CRAPISH_TOK_DELIM);
	}

	tokens[position] = NULL;
	return tokens;
}

int crapish_launch(char** args) {
	pid_t pid, work_pid;
	int status;

	pid = fork();
	if (pid == 0) {
		//child process
		if (execvp(args[0], args) == -1) {
			perror("CRAPiSh");
		}
		exit(EXIT_FAILURE);
	}
	else if (pid < 0) {
		//error forking
		perror("CRAPiSh");
	}
	else {
		//parent process
		do {
			work_pid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}

int crapish_execute(char** args) {

}

void crapish_loop(void) {
	char* line;
	char** args;
	int status;

	do {
		printf("> ");
		line = crapish_read_line();
		args = crapish_split_line(line);
		status = crapish_execute(args);

		free(line);
		free(args);
	} while (status);
}

int main(int argc, char** argv) {

	crapish_loop();

	return EXIT_SUCCESS;
}

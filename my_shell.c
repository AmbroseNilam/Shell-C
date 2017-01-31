#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#define FAILURE -1
#define SUCCESS 1
#define EXIT 0

#define TRUE 1
#define FALSE 0

extern char **getln();

int pid_counter = 0;
int pid_list[256];

int execute();

int handle_process(char**, int, int, char*);

int is_background_mode(char**, int);

int arr_length(void**);

int handle_add(char**, int, int, int);

int list_args(char**, int, int);

void kill_process(int);

void handle_quit(int);

int is_file_output(char**, int, char**);

int main() {
	int ex_ = -1;
	

	do {

		ex_ = execute();
	} while (ex_ != EXIT);
	return 0;
}

int execute() {
	
	sigset(SIGINT, handle_quit);
	sigset(SIGCHLD, kill_process);

	printf("%s ", ">");

	char** argv = getln();
	char* file_name;

	int argc = arr_length((void**) argv);
	int bg_mode = is_background_mode(argv, argc);
	int file_out = is_file_output(argv, argc, &file_name);

	if (strcmp(argv[0], "exit") == 0)
	{
		return EXIT;
	} 
	else if (strcmp(argv[0], "add") == 0)
	{
		return handle_add(argv, argc, FALSE, file_out);
	}
	else if (strcmp(argv[0], "args") == 0)
	{
		return list_args(argv, argc, file_out);
	}
	else if (strcmp(argv[0], "avg") == 0)
	{
		return handle_add(argv, argc, TRUE, file_out);
	}

 	return handle_process(argv, bg_mode, file_out, file_name);
}

int handle_process(char** argv, int bg_mode, int file_out, char* file_name)
{

	int pid_1 = fork();

	if (pid_1 == 0) /* Child Process Here. */
	{
		FILE *outFile;
		outFile = freopen(file_name, "w", stdout);
		int check_exec = execvp(argv[0], argv);
		fclose(outFile);

		if (check_exec == -1)
		{
			printf("%s %d Command not found!\n", argv[0], check_exec);
			exit(FAILURE);
		}

	}
	else if (pid_1 > 0) /* Parent Process Here for Background thread. */ 
	{
		if (bg_mode == TRUE)
		{
			pid_list[pid_counter ++ ] = pid_1;
			/* Background Jobs */
		}
		else
		{
			/* Wait for child to finish exection of command. */
			waitpid(pid_1, 0, 0);
		}

	}
	else
	{
		/* Forking Error Here. */
		printf("%s\n", "Error!");
		exit(FAILURE);
	}

	return SUCCESS;
}

int handle_add(char** argv, int argc, int divi, int file_out)
{
	int total = 0;

	for (int i = 1; i < argc; i++)
	{

		if (i == (argc - 1))
		{
			if (divi == TRUE)
			{
				printf("%s / %d = ", argv[i], (argc - 1));
			}
			else
			{
				printf("%s = ", argv[i]);
			}
		}
		else
		{
			printf("%s + ", argv[i]);
		}

		int val = atoi(argv[i]);

		if (argv[i][1] == 'x')
		{
			sscanf(argv[i], "%x", &val);
		}

		total += val;
	}


	if (divi == TRUE)
	{
		printf("%.2f\n", (float) (total / (argc - 1)));
	}
	else
	{
		printf("%d\n", total);		
	}

	return SUCCESS;

}

int list_args(char** argv, int argc, int file_out)
{
	printf("argc = %d, args = ", argc - 1);
	for (int i = 0; i < argc; i++)
	{
		if (i == (argc - 1))
		{
			printf("%s\n", argv[i]);
		}
		else
		{
			printf("%s, ", argv[i]);
		}
	}
	return SUCCESS;
}

void handle_quit(int sig) {
	fprintf(stdout, "\n%s (%d)\n", "Use 'exit'", sig);
}

int arr_length(void** arr)
{
	int i = 0;
	while (arr[i] != NULL)
	{
		i++;
	}

	return i;
}

int is_file_output(char** argv, int argc, char** file_name)
{
	if (argc < 3)
		return FALSE;

	if (strcmp(argv[argc - 2], ">") == 0)
	{
		*file_name = argv[argc - 1];
		argv[argc - 2] = NULL;
		return TRUE;
	}

	return FALSE;
}

void kill_process(int sig)
{
	for (int i = 0; i < pid_counter; i++)
	{
		printf("Killing %d\n", pid_list[i]);
		kill(pid_list[i], SIGKILL);
	}
}

int is_background_mode(char** argv, int argc)
{
	if (strcmp(argv[argc - 1], "&") == 0)
	{
		argv[argc - 1] = NULL;
		return TRUE;
	}

	return FALSE;
}

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>



int counter = 0;
float scale = 2.0000000;

int forkCounter = 0;
int main(int argc, char* argv[]) 
{
	if(argc != 2) 
	{
		printf("usage: mandelmovie <number of forks>\n");
		exit(1);
	}

	int forkCount = atoi(argv[1]);

	pid_t pid; // Parent process id
	int status;

	counter = 0;

	float step = expf(logf(0.000005/scale)/50);

	// Array of strings
	char* commands[100];

	// Command is ./mandel -x 0.135, -y 0.60 -W 900 -H 900 -m 1000 -s 0.000005 
	time_t end;
	double time_spent;


	// Making the initial commands array for execvp
	commands[0] = "./mandel";
	commands[1] = "-x 0.135";
	commands[2] = "-y 0.60";
	commands[3] = "-W 1024";
	commands[4] = "-H 1024";
	commands[5] = "-m 1000";
	commands[6] = malloc(100 * sizeof(char));	
	commands[7] = "-o";
	commands[8] = malloc(100 * sizeof(char));	
	commands[9] = NULL;


	sprintf(commands[6], "-s %f", scale);		// scale is updated with the original value
	sprintf(commands[8], "mandel%d.bmp", counter);	//The filename is updated with the counter value appended


	// Looping  to execute
	while(counter < 50) {

		// Fork a new process if the fork counter is less than defined amount
		if(forkCounter < forkCount) 
		{
			forkCounter++;
			counter++;
			scale = scale*step;
			sprintf(commands[6], "-s %f", scale);
			sprintf(commands[8], "mandel%d.bmp", counter);

			pid = fork();

			if(pid >= 0) 
			{
				if(pid == 0) 
				{
					int execResult = execvp(commands[0], commands);
					if(execResult == -1) {
						printf("mandel: command error: %s\n", strerror(errno));
						exit(EXIT_FAILURE);
					}
				}
			}

			else {
				perror("Forking error!");
				exit(1);
			}
		}

		// Else, wait to exit
		else {
			
			pid_t result = wait(&status);

			if(result == -1) 
			{
				printf("mandelmovie: wait error %s\n", strerror(errno));
				exit(1);			
			}

			else 
			{
				forkCounter--;

				// Decrement number of forks
				if(status == 0) 
				{
					printf("mandelmovie: process %d exited normally with status %d\n", result, WEXITSTATUS(status));
				}

				else 
				{
					printf("mandelmovie: process %d exited abnormally with status %d\n", result, status);
				}
			}
		}
	}
	end = time(NULL);
	time_spent = end - begin;
	printf("Time spent executing MandelMovie: %lf seconds. \n", time_spent);
	return 0;
}
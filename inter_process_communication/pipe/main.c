#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MESSAGE_SIZE 128
#define CHILDREN_COUNT 4

int main(void)
{
	int parent_to_child_pipes[CHILDREN_COUNT][2];
	int child_to_parent_pipes[CHILDREN_COUNT][2];
	
	// Create pipes
	for (int i = 0; i < CHILDREN_COUNT; i++)
	{
		if (pipe(parent_to_child_pipes[i]) != 0 && pipe(child_to_parent_pipes[i]) != 0)
		{
			fprintf(stderr, "Failed creating parent->child and child->parent pipes\n\n");
			exit(EXIT_FAILURE);
		}
	}
	
	// Fork child processes
	for (int i = 0; i < CHILDREN_COUNT; i++)
	{	
		pid_t child_pid = fork();

		if (child_pid == -1)
		{
			fprintf(stderr, "Failed forking child process\n\n");
			exit(EXIT_FAILURE);
		}
		
		// Child process
		if (child_pid == 0)
		{
			// Close unused pipe ends
			for (int j = 0; j < CHILDREN_COUNT; j++)
			{
				if (j != i)
				{
					// Unused parent-to-child and child-to-parent pipes between
					// other child processes and the parent process
					close(parent_to_child_pipes[j][0]); // Close reading end 
					close(parent_to_child_pipes[j][1]); // Close writing end
					close(child_to_parent_pipes[j][0]); // Close reading end
					close(child_to_parent_pipes[j][1]); // Close writing end
				}
			}
			close(parent_to_child_pipes[i][1]); // Close unused writing end
			close(child_to_parent_pipes[i][0]); // Close unused reading end 

			unsigned char read_buffer[MESSAGE_SIZE];
			unsigned char write_buffer[MESSAGE_SIZE];

			read(parent_to_child_pipes[i][0], read_buffer, MESSAGE_SIZE);
			printf("Child %d (PID %d) recieved: %s\n", i, getpid(), read_buffer);

			sprintf(write_buffer, "Hello parent, this is child %d (PID %d)", i, getpid());
			write(child_to_parent_pipes[i][1], write_buffer, MESSAGE_SIZE);

			close(parent_to_child_pipes[i][0]); // Close used reading end
			close(child_to_parent_pipes[i][1]); // Close used writing end
			exit(EXIT_SUCCESS);
		}
	}

	// Parent process
	// Close unused pipe ends
	for (int i = 0; i < CHILDREN_COUNT; i++)
	{
		close(parent_to_child_pipes[i][0]); // Close unused reading end
		close(child_to_parent_pipes[i][1]); // Close unused writing end
	}

	// Send messages to specific child processes
	for (int i = 0; i < CHILDREN_COUNT; i++)
	{
		unsigned char write_buffer[MESSAGE_SIZE];
		snprintf(write_buffer, MESSAGE_SIZE, "Parent message for child %d", i);
		write(parent_to_child_pipes[i][1], write_buffer, MESSAGE_SIZE);
	}

	// Close used pipes writing end
	for (int i = 0; i < CHILDREN_COUNT; i++)
	{
		close(parent_to_child_pipes[i][1]);
	}

	for (int i = 0; i < CHILDREN_COUNT; i++)
    {
   		unsigned char read_buffer[MESSAGE_SIZE];
        read(child_to_parent_pipes[i][0], read_buffer, MESSAGE_SIZE);
        printf("Recieved message from child %d: %s\n", i, read_buffer);
    }

	for (int i = 0; i < CHILDREN_COUNT; i++)
	{
		wait(NULL);
	}
	
	printf("PARENT (PID = %d): All children processes have finished\n", getpid());
	return 0;
}

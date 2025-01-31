#include <stdio.h>      // Standart I/O functions
#include <stdlib.h>     // Standart library
#include <unistd.h>     // Unix system call
#include <fcntl.h>      // File control options

#define BUFFER_SIZE 4096

int main()
{
    int pipe_fds[2];
    pid_t pid;
    
    // New pipe
    if (pipe(pipe_fds) == -1)
    {
        perror("Failed to create a pipe");
        exit(EXIT_FAILURE);
    }
    
    // New process
    pid = fork();
    if (pid == -1)
    {
        perror("Failed to fork a new process");
        exit(EXIT_FAILURE);
    }
    
    // Child process
    // Purpose: plays audio using 'aplay'
    if (pid == 0)
    {
        close(pipe_fds[1]); // Close unused write end

        // Redirect pipe input to stdin
        dup2(pipe_fds[0], STDIN_FILENO);
        close(pipe_fds[0]); // Close read end

        // Execute 'aplay' to play audio from stdin
        execlp("aplay", "aplay", NULL);

        perror("execlp failed");
        exit(EXIT_FAILURE);
    }
    
    // Parent process
    // Purpose: reads audio file and writes to pipe
    else
    {
        close(pipe_fds[0]); // Close unused read end

        // Open an audio file (WAV format)
        int audio_fd = open("audio_sample.wav", O_RDONLY);
        if (audio_fd == -1)
        {
            perror("Failed to open audio file\n");
            exit(EXIT_FAILURE);
        }

        char buffer[BUFFER_SIZE];
        ssize_t bytes_read;

        // Read file and write to pipe
        while ((bytes_read = read(audio_fd, buffer, BUFFER_SIZE)) > 0)
        {
            write(pipe_fds[1], buffer, bytes_read);
        }

        close(audio_fd);
        close(pipe_fds[1]); // Close write end
    }

    return 0;
}


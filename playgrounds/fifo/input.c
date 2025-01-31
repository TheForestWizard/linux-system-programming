#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <termios.h>

#define FIFO_NAME "game_fifo"

void set_input_mode()
{
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO); // Disable buffering and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

int main()
{
    set_input_mode();

    int fifo_fd = open(FIFO_NAME, O_WRONLY); // Open FIFO for writing
    if (fifo_fd == -1)
    {
        perror("Failed to open FIFO");
        exit(1);
    }

    char move;
    printf("Enter moves (WASD), Q to quit:\n");

    while (1) 
    {
        move = getchar(); // Get user input
        if (move == 'q' || move == 'Q')
        {
            write(fifo_fd, &move, 1);
            break;
        }

        if (move == 'w' || move == 'a' || move == 's' || move == 'd')
        {
            write(fifo_fd, &move, 1); // Send input to game;
        }
    }

    close(fifo_fd);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define GRID_SIZE 8
#define FIFO_NAME "game_fifo"

// Player starts at center
int player_x = 4;
int player_y = 4;

void draw_grid()
{
    system("clear"); // Clear terminal
    for (int y = 0; y < GRID_SIZE; y++)
    {
        for (int x = 0; x < GRID_SIZE; x++)
        {
            if (x == player_x && y == player_y)
            {
                printf(" P "); // Player
            }

            else
            {
                printf(" . "); // Empty space
            }
        }
        printf("\n");
    }
    printf("Use W/A/S/D to move. Press Q to quit.\n");
}

int main()
{
    mkfifo(FIFO_NAME, 0666); // Create FIFO file

    int fifo_fd = open(FIFO_NAME, O_RDONLY); // Open FIFO for reading
    if (fifo_fd == -1)
    {
        perror("Failed to open FIFO\n");
        exit(1);
    }

    char move;
    draw_grid(); // Initial grid

    while (1)
    {
        if (read(fifo_fd, &move, 1) > 0)
        {
            if (move == 'q' || move == 'Q') break; // Exit game

            // Update position based on input
            if (move == 'w' && player_y > 0) player_y--;
            if (move == 's' && player_y < GRID_SIZE -1) player_y++;
            if (move == 'a' && player_x > 0) player_x--;
            if (move == 'd' && player_x < GRID_SIZE - 1) player_x++;

            draw_grid(); // Redraw grid after moving
        }
    }

    close(fifo_fd);
    unlink(FIFO_NAME); // Remove FIFO file
    printf("Game Over!\n");
    return 0;
}

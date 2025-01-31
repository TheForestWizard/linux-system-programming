#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <string.h>
#include <signal.h>

#define GRID_SIZE 8
#define SHM_NAME "/game_shm"
#define SEM_NAME "/game_sem"

typedef struct
{
	char grid[GRID_SIZE][GRID_SIZE];
} GameState;

void draw_grid(GameState* game)
{
	system("clear");
	for (int y = 0; y < GRID_SIZE; y++)
	{
		for (int x = 0; x < GRID_SIZE; x++)
		{
			printf(" %c ", game->grid[y][x]);
		}
		printf("\n");
	}
	printf("Use W/A/S/D to move. Press Q to quit.\n");
}

int main()
{
	int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd, sizeof(GameState));
	
	GameState* game = mmap(0, sizeof(GameState), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	sem_t* sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);

	// Initialize grid
	sem_wait(sem);
	memset(game->grid, '.', sizeof(game->grid));
	sem_post(sem);

	while (1)
	{
		sem_wait(sem); // Lock the grid
		draw_grid(game);
		sem_post(sem); // Unlock the grid
		usleep(100000); // Delay to refresh screen
	}

	sem_close(sem);
	sem_unlink(SEM_NAME);
	shm_unlink(SHM_NAME);
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <termios.h>
#include <time.h>

#define GRID_SIZE 8
#define SHM_NAME "/game_shm"
#define SEM_NAME "/game_sem"

typedef struct
{
	char grid[GRID_SIZE][GRID_SIZE];
} GameState;

void set_input_mode()
{
	struct termios t;
	tcgetattr(STDIN_FILENO, &t);
	t.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

int main()
{
	set_input_mode();
	srand(time(NULL) ^ getpid());

	int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
	if (shm_fd == -1)
	{
		perror("shm_open failed");
		return 1;
	}

	GameState* game = mmap(0, sizeof(GameState), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (game == MAP_FAILED)
	{
		perror("mmap failed");
		return 1;
	}

	sem_t* sem = sem_open(SEM_NAME, 0);
	if (sem == SEM_FAILED)
	{
		perror("sem_open failed");
		return 1;
	}

	char player_symbol = 'A' + (getpid() % 26);
	int x = rand() % GRID_SIZE;
	int y = rand() % GRID_SIZE;

	sem_wait(sem);

	if (game->grid[y][x] == '.')
	{
		game->grid[y][x] = player_symbol;
	}

	else
	{
		for (int i = 0; i < GRID_SIZE; i++)
		{
			for (int j = 0; j < GRID_SIZE; j++)
			{
				if (game->grid[i][j] == '.')
				{
					x = j;
					y = i;
					game->grid[y][x] = player_symbol;
					break;
				}
			}
		}
	}
	sem_post(sem);

	while (1)
	{
		char move = getchar();
		if (move == 'q' || move == 'Q') break;

		sem_wait(sem);

		if (game->grid[y][x] == player_symbol)
		{
			game->grid[y][x] = '.';
		}

		if (move == 'w' && y > 0) y--;
		if (move == 's' && y < GRID_SIZE - 1) y++;
		if (move == 'a' && x > 0) x--;
		if (move == 'd' && x < GRID_SIZE - 1) x++;

		if (game->grid[y][x] == '.');
		{
			game->grid[y][x] = player_symbol;
		}

		sem_post(sem); // Unlock
		usleep(50000);
	}

	sem_close(sem);
	return 0;
}

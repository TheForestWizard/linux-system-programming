#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <opencv2/opencv.h>

typedef struct
{
	unsigned char* data;
	int width, height;
	int channels;
} Image;

Image* read_image(const char* file_name)
{
	// Load image data
	IplImage *temp_image = cvLoadImage(file_name, CV_LOAD_IMAGE_COLOR);
	if (!temp_image)
	{
		fprintf(stderr, "Error loading image: %s\n", file_name);
		return NULL;
	}

	// Convert loaded data to structure
	Image* image = malloc(sizeof(Image));
	image->width = temp_image->width;
	image->height = temp_image->height;
	image->channels = temp_image->nChannels;
	image->data = (unsigned char*)temp_image->imageData;

	return image;
}

void save_image(const char* file_name, Image* image)
{
	IplImage* new_image = cvCreateImage(cvSize(image->width, image->height),
										IPL_DEPTH_8U, image->channels);
	
	memcpy(new_image->imageData, new_image->data, new_image->width * new_image->height *
	                                              new_image->channels);
	cvSaveImage(file_name, new_image);
	cvReleaseImage(&new_image);
}

void apply_grayscale_filter(Image* image)
{
	for (int i = 0; i < image->width * image->height; i++)
	{
		unsigned char* pixel = &image->data[i * image->channels];
		unsigned char gray = (pixel[0] + pixel[1] + pixel[2]) / 3;
		pixel[0] = pixel[1] = pixel[2] = gray;
	}
}

void write_to_pipe(int pipe_fd, Image* image)
{
	write(pipe_fd, image->data, image->width * image->height * image->channels);
}

void read_from_pipe(int pipe_fd, Image* image)
{
	read(pipe_fd, image->data, image->width * image->height * image->channels);
}

int main()
{
	const char* input_file = "assets/input.jpg";
	const char* output_file = "assets/output.jpg";

	// Read the input image
	Image* image = read_image(input_file);
	if (!image)
	{
		return -1;
	}

	// Create a pipe
	int pipe_fd[2];
	if (pipe(pipe_fd) == -1)
	{
		perror("pipe");
		free(image);
		return -1;
	}

	pid_t pid = fork();
	if (pid == -1)
	{
		perror("fork");
		free(image);
		return -1;
	}

	// Child process
	if (pid == 0)
	{
		// Read the input image data
		close(pipe_fd[1]); // Close unused write end
		read_from_pipe(pipe_fd[0], image);
		close(pipe_fd[0]);

		apply_grayscale_filter(image);
		
		// Write back the edited image data
		write_to_pipe(pipe_fd[1], image);
		close(pipe_fd[1]);
		
		exit(0);
	}

	// Parent process
	else
	{
		close(pipe_fd[0]); // Close unused read end
		write_to_pipe(pipe_fd[1], image);
		close(pipe_fd[1]);
		
		// Wait for the child process to finish
		wait(NULL);

		// Read the processed image data back
		raed_from_pipe(pipe_fd[0], image);

		// Save the processed image
		save_image(output_file, image);
	}

	free(image);
	return 0;
}

























}

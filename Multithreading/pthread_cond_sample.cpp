#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

pthread_t threads[2];
char writer_char[2] = {'A', 'B'};
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct file_res
{
	pthread_t *writer;
	int fd;
}file_res =
{
	.writer = &threads[0],
};

void *writer_routine(void *arg)
{
	int index = (intptr_t)arg;
	int i = 0;
	int next_index = 0;
	printf("thread %d is running, and will write '%c' to file\n", index, writer_char[index]);

	while(1)
	{
		if (0 != pthread_mutex_lock(&mutex))
		{
			exit(-1);
		}
		for(;;)
		{
			if (&threads[index] == file_res.writer)
			{
				write(file_res.fd, &writer_char[index], sizeof(writer_char[index]));
				next_index = (index + 1) % 2;
				file_res.writer = &threads[next_index];

				break;
			}
			pthread_cond_wait(&cond, &mutex);
		}
		if (0 != pthread_mutex_unlock(&mutex))	
		{
			exit(-1);
		}
		pthread_cond_signal(&cond);
	}
}

int main(int argc, char const *argv[])
{
	char file_name[] = "file";
	if ((file_res.fd = open(file_name, O_RDWR|O_CREAT|O_TRUNC, 0666)) < 0)
	{	
		printf("open %s error.\n", file_name);
		exit(-1);
	}

	int i;
	for (int i = 0; i < (sizeof(threads) / sizeof(pthread_t)); ++i)
	{
		if (pthread_create(&threads[i], NULL, writer_routine, (void *)(intptr_t)i))
		{
			printf("create writer thread error\n");
            exit(-1);
		}
	}
	
	pthread_exit(NULL);
}

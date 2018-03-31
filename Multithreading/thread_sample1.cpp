#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *thread_routine1(void *arg)
{
	fprintf(stdout, "thread1: hello world!\n");
	sleep(1);

	return NULL;
}

void *thread_routine2(void* arg)
{
	fprintf(stdout, "thread2: I'm running...\n");
	pthread_t main_thread = (pthread_t)arg;

	pthread_detach(pthread_self());

	if (!pthread_equal(main_thread,pthread_self()))
	{
		fprintf(stdout, "thread2: main thread id is not equal thread2\n");
	}

	pthread_join(main_thread, NULL);
	fprintf(stdout, "thread2: main thread exit!\n");

    fprintf(stdout, "thread2: exit!\n");
    fprintf(stdout, "thread2: process exit!\n");

    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
	pthread_t t1;
	if (pthread_create(&t1, NULL, thread_routine1, NULL) != 0)
	{
		fprintf(stderr, "create thread fail.\n");
		exit(-1);
	}

	pthread_join(t1, NULL);
	fprintf(stdout, "main thread: thread1 terminated\n\n");

	pthread_t t2;
	if(pthread_create(&t2, NULL, thread_routine2, (void *)pthread_self()) != 0)
	{
		fprintf(stderr, "create thread fail.\n");
		exit(-1);
	}

	fprintf(stdout, "main thread: sleeping...\n");
	sleep(3);

	fprintf(stdout, "main thread: exit\n");
	pthread_exit(NULL);

	fprintf(stdout, "main thread: never reach here!\n");
	return 0;
}
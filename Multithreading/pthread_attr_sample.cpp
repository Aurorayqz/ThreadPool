#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define handle_error_en(en, msg) \
		do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

static void
display_pthread_attr(pthread_attr_t *attr, char *prefix)
{
	int s, i;
	size_t v;
	void *stkaddr;

	s = pthread_attr_getdetachstate(attr, &i);
	if (s != 0)
	{
		handle_error_en(s, "pthread_attr_getdetachstate");
	}
	printf("%sDetach state        = %s\n", prefix,
			(i == PTHREAD_CREATE_DETACHED) ? "PTHREAD_CREATE_DETACHED" :
			(i == PTHREAD_CREATE_JOINABLE) ? "PTHREAD_CREATE_JOINABLE" :
			"???");

	s = pthread_attr_getstack(attr, &stkaddr, &v);
	if (s != 0)
	{
		handle_error_en(s, "pthread_attr_getstack");
	}
	printf("%sStack address       = %p\n", prefix, stkaddr);
	printf("%sStack size          = 0x%x bytes\n", prefix, (unsigned)v);
}

static void *thread_start(void *arg)
{
	int s;
	pthread_attr_t gattr;

	s = pthread_getattr_np(pthread_self(), &gattr);
	if (s != 0)
	{
		handle_error_en(s, "pthread_getattr_np");
	}

	printf("Thread attributes:\n");
	display_pthread_attr(&gattr, "\t");

	exit(EXIT_SUCCESS);
}

int main(int argc, char const *argv[])
{
	pthread_t thr;
	pthread_attr_t attr;
	pthread_attr_t *attrp;
	int s;

	attrp = NULL;

	if (argc > 1) {
		int stack_size;
		void *sp;

		attrp = &attr;

		s = pthread_attr_init(&attr);
		if (s != 0)
		{
			handle_error_en(s, "pthread_attr_init");
		}

		s = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		if (s != 0)
		{
			handle_error_en(s, "pthread_attr_setdetachstate");
		}

		stack_size = strtoul(argv[1], NULL, 0);

		s = posix_memalign(&sp, sysconf(_SC_PAGESIZE), stack_size);
		if (s != 0)
		{
			handle_error_en(s, "posix_memalign");
		}

		printf("posix_memalign() allocated at %p\n", sp);

		s = pthread_attr_setstack(&attr, sp, stack_size);
		if (s != 0)
		{
			handle_error_en(s, "pthread_attr_setstack");
		}
	}

	s = pthread_create(&thr, attrp, &thread_start, NULL);
	if (s != 0)
	{
		handle_error_en(s, "pthread_create");
	}

	if (attrp != NULL)
	{
		s = pthread_attr_destroy(attrp);
		if (s != 0)
		{
			handle_error_en(s, "pthread_attr_destroy");
		}
	}

	pause();
}
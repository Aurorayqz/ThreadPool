/* filename: main.cpp
*  desp: Calling mystrerror() from two different threads
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#define handle_error_en(en, msg) \
		do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

extern char *mystrerror(int err);

static void *threadFunc(void *arg)
{
	char *str;
	printf("Other thread about to call mystrerror()\n");
	/*子线程获取EPERM错误码的字符信息*/
	str = mystrerror(EPERM);
	printf("Other thread: str (%p) = %s\n", str, str);
	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t t;
	int s;
	char *str;
	/*主线程获取EINVAL错误码的字符信息*/
	str = mystrerror(EINVAL);
	printf("Main thread has called mystrerror()\n");
	s = pthread_create(&t, NULL, threadFunc, NULL);
	if (s != 0)
		handle_error_en(s, "pthread_create");
	s = pthread_join(t, NULL);
	if (s != 0)
		handle_error_en(s, "pthread_join");
	printf("Main thread: str (%p) = %s\n", str, str);

    exit(EXIT_SUCCESS);
}
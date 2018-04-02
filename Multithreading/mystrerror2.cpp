#define _GNU_SOURCE
#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

static pthread_once_t once = PTHREAD_ONCE_INIT;
static pthread_key_t strerrorKey;

#define handle_error_en(en, msg) \
		do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
		do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define MAX_ERROR_LEN 256 

/*线程特有数据的析构函数*/
static void destructor(void *buf)
{
	free(buf);
}


static void createKey(void)
{
	int s; 
	/*在phtread_once函数里创建特有数据的key，
	哪个线程先调用就哪个线程创建key*/
    s = pthread_key_create(&strerrorKey, destructor);
	if (s != 0)
		handle_error_en(s, "pthread_key_create");
}

char *mystrerror(int err)
{
    int s;
    char *buf;

    /*一次性初始化函数*/
    s = pthread_once(&once, createKey);
    if (s != 0)
        handle_error_en(s, "pthread_once");

    /*获取线程特有数据*/
    buf = pthread_getspecific(strerrorKey);
    if (buf == NULL) { /*第一次获取为NULL, 线程需要分配内存*/
        buf = malloc(MAX_ERROR_LEN);
        if (buf == NULL)
            handle_error("malloc");
        /*设置特有数据内存*/
        s = pthread_setspecific(strerrorKey, buf);
        if (s != 0)
            handle_error_en(s, "pthread_setspecific");
    }

    if (err < 0 || err >= _sys_nerr || _sys_errlist[err] == NULL) {
        snprintf(buf, MAX_ERROR_LEN, "Unknown error %d", err);
    } else {
        strncpy(buf, _sys_errlist[err], MAX_ERROR_LEN - 1);
        buf[MAX_ERROR_LEN - 1] = '\0'; /* Ensure null termination */
    }

    return buf;
}
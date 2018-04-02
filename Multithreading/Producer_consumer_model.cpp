#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define ONE_SECOND 1000000
#define RANGE 10
#define PERIOD 2
#define NUM_THREADS 4 

typedef struct {
	int *carpark;	// 用一个整数数组 buffer 模拟停车场停车位
	int capacity;	// 停车场的车辆容量
	int occupied;	// 停车场现有车辆数目
	int nextin;		// 下一个进来的车的停车位置（用 carpark 数组代表的下标表示）
	int nextout;	// 下一个取走的车的停车位置（用 carpark 数组代表的下标表示）
	int cars_in;	// 记录停车场进入车辆的总和
	int cars_out;	//记录从停车场开出去的车辆总和
	pthread_mutex_t lock;	//互斥量，保护该结构中的数据被线程互斥的方式使用
	pthread_cond_t space;	//条件变量，描述停车场是否有空位置
	pthread_cond_t car;		//条件变量，描述停车场是否有车
	pthread_barrier_t bar;	//线程屏障
} cp_t;

static void * car_in_handler(void *cp_in);
static void * car_out_handler(void *cp_out);

static void * monitor(void *cp_in);
static void initialise(cp_t *cp, int size);

int main(int argc, char const *argv[])
{
	if (argc != 2)
	{
		printf("Usage: %s carparksize\n", argv[0]);
        exit(1);
	}

	cp_t ourpark;

	initialise(&ourpark, atoi(argv[1]));
	pthread_t car_in, car_in2, car_out, car_out2, m;

	pthread_create(&car_in, NULL, car_in_handler, (void *)&ourpark);
	pthread_create(&car_out, NULL, car_out_handler, (void *)&ourpark);
	pthread_create(&car_in2, NULL, car_in_handler, (void *)&ourpark);
	pthread_create(&car_out2, NULL, car_out_handler, (void *)&ourpark);
	pthread_create(&m, NULL, monitor, (void *)&ourpark);

	pthread_join(car_in, NULL);
	pthread_join(car_in2, NULL);
	pthread_join(car_out, NULL);
	pthread_join(car_out2, NULL);
	pthread_join(m, NULL);

	exit(0);
}

static void initialise(cp_t *cp, int size)
{
	cp->occupied = cp->nextin = cp->nextout = cp->cars_in = cp->cars_out = 0;
    cp->capacity = size;

    cp->carpark = (int *)malloc(cp->capacity * sizeof(*cp->carpark));

    pthread_barrier_init(&cp->bar, NULL, NUM_THREADS);

    if (cp->carpark == NULL)
    {
    	perror("malloc()");
    	exit(1);
    }

    srand((unsigned int)getpid());

    pthread_mutex_init(&cp->lock, NULL);
    pthread_cond_init(&cp->space, NULL);
    pthread_cond_init(&cp->car, NULL);
}

static void * car_in_handler(void *carpark_in)
{
	cp_t *temp;
	unsigned int seed;
	temp = (cp_t *)carpark_in;

	pthread_barrier_wait(&temp->bar);
	while (1){

		// 将线程随机挂起一段时间，模拟车辆到来的的随机性
		usleep(rand_r(&seed) % ONE_SECOND);

		pthread_mutex_lock(&temp->lock);

		while (temp->occupied == temp->capacity)
			pthread_cond_wait(&temp->space, &temp->lock);

		temp->carpark[temp->nextin] = rand_r(&seed) % RANGE;

		temp->occupied++;
        temp->nextin++;
        temp->nextin %= temp->capacity;

        temp->cars_in++;

        pthread_cond_signal(&temp->car);

		pthread_mutex_unlock(&temp->lock);
	}
	return ((void *)NULL);
}

static void * car_out_handler(void *carpark_out)
{
	cp_t *temp;
	unsigned int seed;
	temp = (cp_t *)carpark_out;
	pthread_barrier_wait(&temp->bar);
	while (1){
		usleep(rand_r(&seed) % ONE_SECOND);

		pthread_mutex_lock(&temp->lock);

		while (temp->occupied == 0)
			pthread_cond_wait(&temp->car, &temp->lock);

		temp->occupied--;
		temp->nextout++;
		temp->nextout %= temp->capacity;
		temp->cars_out++;

		pthread_cond_signal(&temp->space);

		pthread_mutex_unlock(&temp->lock);
	}
	return ((void *)NULL);
}

static void * monitor(void *carpark_in)
{
	cp_t *temp;
	temp = (cp_t *)carpark_in;

	while (1) {
		sleep(PERIOD);

		pthread_mutex_lock(&temp->lock);

		printf("Delta: %d\n", temp->cars_in - temp->cars_out - temp->occupied);
		printf("Number of cars in carpark: %d\n", temp->occupied);

		pthread_mutex_unlock(&temp->lock);

	}

	return ((void *)NULL);
}
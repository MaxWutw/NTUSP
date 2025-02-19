#pragma once

#include <pthread.h>
#define MAX_QUEUE_SIZE 100

/* You may define additional structures / typedef's in this header file if
 * needed.
 */
typedef struct{
	Martrix a, b, c;
	int32_t start, end, n;
}work_t;

typedef struct tpool {
  // Add things you need here
	pthread_mutex_t lock;
	pthread_cond_t cond;

	pthread_t frontend;
	pthread_t *backend;

	work_t work_queue[MAX_QUEUE_SIZE];

	int32_t num_threads;
	int32_t queue_size;
	int32_t front, rear;
	int32_t n;
}tpool_t;

typedef int** Matrix;
typedef int* Vector;

struct tpool* tpool_init(int num_threads, int n);
void tpool_request(struct tpool*, Matrix a, Matrix b, Matrix c, int num_works);
void tpool_synchronize(struct tpool*);
void tpool_destroy(struct tpool*);
int calculation(int n, Vector, Vector);  // Already implemented

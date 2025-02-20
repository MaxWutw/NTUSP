#include "tpool.h"

#include <pthread.h>

void *worker_thread(void *arg){
	tpool_t *pool = (tpool_t*)arg;
	while(1){
		pthread_mutex_lock(&pool->lock);
		while(pool->queue_size == 0 && !pool->terminate){
			pthread_cond_wait(&pool->cond, &pool->lock);
		}
		work_t *work = pool->work_queue[pool->front];
		pthread_mutex_lock(&pool->lock);
		pool->front = (pool->front + 1) % MAX_QUEUE_SIZE;
		pool->queue_size--;
		pthread_mutex_unlock(&pool->lock);

		for(int32_t i = work->start;i < work->end;i++){
			int32_t row = i / work->n;
			int32_t col = i % work->n;
			work->c[row][col] = calculation(work->n, work->a[row], work->b[col]);
		}
		free(work);
	}
	return NULL;
}

struct tpool *tpool_init(int num_threads, int n) {
	tpool_t *pool = (tpool_t *)malloc(sizeof(tpool_t));
	pool->num_threads = num_threads;
	pthread_mutex_init(&pool->lock, NULL);
	pthread_cond_init(&pool->cond, NULL);
	pool->n = n;
	pool->rear = 0;
	pool->front = 0;
	pool->queue_size = 0;
	pool->terminate = 0;
	pool->backend_threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
	for(int32_t i = 0;i <= num_threads;i++){
		pthread_create(&pool->backend_threads[i], NULL, worker_thread, pool);
	}

	return pool;
}

void tpool_request(struct tpool *pool, Matrix a, Matrix b, Matrix c,
                   int num_works) {
	Matrix transpose_b = (Matrix)malloc(pool->n * sizeof(Vector));
	for(int32_t i = 0;i < pool->n;i++){
		transpose_b[i] = (Vector)malloc(pool->n * sizeof(int32_t));
		for(int32_t j = 0;j < pool->n;j++){
			transpose_b[i][j] = b[j][i];
		} 
	}
	int32_t total = pool->n * pool->n;
	int32_t work_size = total / num_works;
	int32_t remainder = total % num_works;
	int32_t start = 0;
	for(int32_t i = 0;i < num_works;i++){
		int32_t end = start + work_size + (i < remainder ? 1 : 0);
		work_t *work = (work_t *)malloc(sizeof(work_t));
		work->start = start;
		work->end = end;
		work->a = a;
		work->b = transpose_b;
		work->c = c;
		work->n = pool->n;

		pthread_mutex_lock(&pool->lock);
		pool->work_queue[pool->rear] = work;
		pool->rear = (pool->rear + 1) % MAX_QUEUE_SIZE;
		pool->queue_size++;
		pthread_cond_signal(&pool->cond);
		pthread_mutex_unlock(&pool->lock);

		start = end;
	}
}

void tpool_synchronize(struct tpool *pool) {
	pthread_mutex_lock(&pool->lock);
	while(pool->queue_size > 0){
		pthread_cond_wait(&pool->cond, &pool->lock);
	}
	pthread_mutex_unlock(&pool->lock);
}

void tpool_destroy(struct tpool *pool) {
	pthread_mutex_lock(&pool->lock);
	pool->terminate = 1;
	pthread_cond_broadcast(&pool->cond);
	pthread_mutex_unlock(&pool->lock);

	for(int32_t i = 0;i <= pool->num_threads;i++){
		pthread_join(pool->backend_threads[i], NULL);
	}
	free(pool->backend_threads);
	pthread_mutex_destroy(&pool->lock);	
	pthread_cond_destroy(&pool->cond);
	free(pool);
}

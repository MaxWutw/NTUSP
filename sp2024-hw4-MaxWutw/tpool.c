#include "tpool.h"

#include <pthread.h>

void *worker_thread(void *arg){
	tpool_t *pool = (tpoll_t*)arg;
	while(1){
		pthread_mutex_lock(&pool->lock);
		while(pool->queue_size == 0 && !pool->stop){
			pthread_cond_wait(&pool->cond, &pool->lock);
		}
		work_t *work = pool->work_queue[pool->front];
		pool->front = (pool->front + 1) % MAX_QUEUE_SIZE;
		pool->queue_size--;
		pthread_mutex_unlock(&pool->lock);

		for(int32_t i = work->start;i < work->end;i++){

		}
		free(work);
	}
	return NULL;
}

struct tpool *tpool_init(int num_threads, int n) {
	tpool_t *pool = (tpool_t *)malloc(sizeof(tpool_t));
	pool->num_threads = num_threads;
	pthread_mutex_init(pool->lock, NULL);
	pthread_cond_init(pool->cond, NULL);
	pool->n = n;
	pool->threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
	for(int32_t i = 0;i < num_threads;i++){
		pthread_create(&pool->threads[i], NULL, worker_thread, pool);
	}

	return pool;
}

void tpool_request(struct tpool *pool, Matrix a, Matrix b, Matrix c,
                   int num_works) {
}

void tpool_synchronize(struct tpool *pool) {
}

void tpool_destroy(struct tpool *pool) {
}

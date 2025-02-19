#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#define MAX_COUNT 3 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int32_t buffer[MAX_COUNT];
int32_t cnt = 0;
void *server(void *arg){
	for(int32_t i = 0;i < 3;i++){
		pthread_mutex_lock(&mutex);

		while(cnt == MAX_COUNT){
			pthread_cond_wait(&cond, &mutex);
			printf("server wait\n");
			usleep(1000);
		}

		buffer[cnt++] = i;
		printf("Produced %d\n", i);

		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
		usleep(100000);
	}
	printf("server out\n");
	return NULL;
}

void *client(void *arg){
	for(int32_t i = 0;i < 3;i++){
		pthread_mutex_lock(&mutex);

		while(cnt == 0){
			pthread_cond_wait(&cond, &mutex);
			printf("client wait\n");
			usleep(1000);
		}
		int32_t item = buffer[--cnt];
		printf("Consumed: %d\n", item);
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
	}
	printf("client out\n");
	return NULL;
}
int main(){
	pthread_t cli, ser;
	pthread_create(&ser, NULL, server, NULL);
	pthread_create(&cli, NULL, client, NULL);

	pthread_join(cli, NULL);
	pthread_join(ser, NULL);

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);

	return 0;
}

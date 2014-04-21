#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>

struct philosopher {
	int nums;
	pthread_t thread;
	struct fork *left, *right;
	int times_to_eat;
};

struct fork {
	int nums;
	pthread_mutex_t mutex;
};

void* philosopher_behavior(struct philosopher* p) {
	int i;
	for (i = 0; i < p->times_to_eat; i++) {
		srandom(time(NULL));
		int time = rand() % 3 + 1;
		sleep(time); // думает
		printf("Философ %d думал %d секунд(у)\n", p->nums + 1, time);
		pthread_mutex_lock(&p->right->mutex);
		printf("Философ %d взял вилку %d (правую)\n", p->nums + 1, p->right->nums + 1);
		if (pthread_mutex_trylock(&p->left->mutex)) {
			pthread_mutex_unlock(&p->right->mutex);
			i--;
			printf("Не удалось взять левую вилку философу %d \n", p->nums + 1);
			continue;
		}
		printf("Философ %d взял вилку %d (левую)\n", p->nums + 1, p->left->nums + 1);
		time = rand() % 3 + 1;
		sleep(time); // ест
		printf("Философ %d ел %d секунд(у)\n", p->nums + 1, time);
		pthread_mutex_unlock(&p->left->mutex);
		printf("Философ %d положил вилку %d (левую)\n", p->nums + 1, p->left->nums + 1);
		pthread_mutex_unlock(&p->right->mutex);
		printf("Философ %d положил вилку %d (правую)\n", p->nums + 1, p->right->nums + 1);
	}
	return 0;
}

int main (int argc, char* argv[]) {
	int N = atoi(argv[1]);
	if (argc != N + 2) {
		fprintf(stderr, "Неверное количество аргументов\n");
		return 1;
	}
	struct philosopher philosophers[N];
	struct fork forks[N];
	int i;
	for (i = 0; i < N; i++) {
		philosophers[i].times_to_eat = atoi(argv[i + 2]);
	}
	for (i = 0; i < N; i++) {
		forks[i].nums = i;
		pthread_mutex_init(&forks[i].mutex, NULL);
	}
	for (i = 0; i < N; i++) {
		philosophers[i].nums = i;
		philosophers[i].left = &forks[i % N];
		philosophers[i].right = &forks[(i + 1) % N];
	}
	for (i = 0; i < N; i++) {
		pthread_create(&philosophers[i].thread, NULL, (void* (*) (void *)) philosopher_behavior, &philosophers[i]);
	}
	void* result;
	for (i = 0; i < N; i++) {
		pthread_join(philosophers[i].thread, &result);
	}
	for (i = 0; i < N; i++) {
		pthread_mutex_destroy(&forks[i].mutex);
	}
	return 0;
}

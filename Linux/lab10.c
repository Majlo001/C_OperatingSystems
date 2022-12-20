#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>


pthread_mutex_t mutexLock = PTHREAD_MUTEX_INITIALIZER;

struct threadInfo {
	int n;
	int first;
};


void *thread(void *data){
	pthread_mutex_lock(&mutexLock);
	struct threadInfo *ti = (struct threadInfo*) data;

	pthread_t self = pthread_self();
	srand(self);
	fprintf(stdout, "Thread #%lx, size = %d, first = %d\n", self, (*ti).n, (*ti).first);

	double val = 1.0;
	double* res = malloc(sizeof(double));
	for(double i=(*ti).first; i<=((*ti).first + (*ti).n); i++){
		val = val * (double)((2*i)*(2*i))/(double)(((2*i) - 1)*((2*i) + 1));
	}
	*res = val;
	free(ti);
	pthread_mutex_unlock(&mutexLock);
	return (void *) res;
}


int main(int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr, "Too many arguments\n");
		return 1;
	}

	for(int x=1; x<=2; x++){
		int n=strlen(argv[x]);
		for(int i=0; i<n; i++){
			if (!isdigit(argv[x][i])) {
			  fprintf(stderr, "Argument %d is not integer\n", x);
			  return 1;
			}
		}
	}



	int n = atoi(argv[1]);
	int w = atoi(argv[2]);
	if (n < 1 || n > 1000000000) {
		fprintf(stderr, "Agument n is not from interval <1, 10e9>\n");
		return 1;
	}
	if (w < 1 || w > 100) {
		fprintf(stderr, "Agument w is not from interval <1, 100>\n");
		return 1;
	}
	struct timespec ts1start={0,0}, ts1end={0,0};
	struct timespec ts2start={0,0}, ts2end={0,0};


	pthread_mutex_init(&mutexLock, NULL);

	pthread_t threads[w];
	struct threadInfo *ti;


	clock_gettime(CLOCK_MONOTONIC, &ts1start);
	if(n % w != 0){
		for(int i=0; i<w-1; i++){
			ti = malloc(sizeof(struct threadInfo));
			(*ti).n = n/w;
			(*ti).first = (n/w)*i + 1;
			pthread_create(threads+i, NULL, thread, (void *) ti);
		}
		ti = malloc(sizeof(struct threadInfo));
		(*ti).n = (n/w)+(n%w);
		(*ti).first = (n/w)*(w-1) + 1;
		pthread_create(threads+(w-1), NULL, thread, (void *) ti);

	} else {
		for(int i=0; i<w; i++){
			ti = malloc(sizeof(struct threadInfo));
			(*ti).n = n/w;
			(*ti).first = (n/w)*i + 1;
			pthread_create(threads+i, NULL, thread, (void *) ti);
		}
	}

	double wynik = 1.0;
	for(int i=0; i<w; i++){
		double* retval;
		pthread_join(threads[i], (void **) &retval);
		wynik = wynik * *retval;
	}
	wynik = wynik*2.0;
	clock_gettime(CLOCK_MONOTONIC, &ts1end);


	clock_gettime(CLOCK_MONOTONIC, &ts2start);
	double wynik2 = 1.0;
	for(double i=1; i<n; i++){
		wynik2 = wynik2 * ((2*i)*(2*i))/(((2*i) - 1)*((2*i) + 1));
	}
	wynik2 = wynik2*2.0;
	clock_gettime(CLOCK_MONOTONIC, &ts2end);


	fprintf(stdout, "Wynik w/Threads: %.15lf time: %.5f sec\n", wynik, ((double)ts1end.tv_sec + 1.0e-9*ts1end.tv_nsec) - ((double) ts1start.tv_sec + 1.0e-9*ts1start.tv_nsec));
	fprintf(stdout, "Wynik wo/Threads: %.15lf time: %.5f sec\n", wynik2, ((double)ts2end.tv_sec + 1.0e-9*ts2end.tv_nsec) - ((double) ts2start.tv_sec + 1.0e-9*ts2start.tv_nsec));

	pthread_mutex_destroy(&mutexLock);
	return 0;
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <windows.h>
#include <math.h>


HANDLE mutex;

struct threadInfo {
	int n;
	int first;
	double retval;
};


DWORD WINAPI thread(LPVOID data) {

	struct threadInfo* ti = (struct threadInfo*)data;


	DWORD self = GetCurrentThreadId();
	srand(self);

	WaitForSingleObject(mutex, INFINITE);
	double val = 0.0, temp;
	for (double i = ti->first; i <= (ti->first + ti->n); i++) {
		temp = (double)pow(-1, i)/ (double)(2*i + 1);
		val += temp;
	}
	//fprintf(stdout, "%d %d %f\n", ti->first, ti->n, val);
	ti->retval = val;
	ReleaseMutex(mutex);
	return 0;
}


int main(int argc, char** argv) {
	if (argc != 3) {
		fprintf(stderr, "Too many arguments\n");
		return 1;
	}
	for (int x = 1; x <= 2; x++) {
		int n = strlen(argv[x]);
		for (int i = 0; i < n; i++) {
			if (!isdigit(argv[x][i])) {
				fprintf(stderr, "Argument %d is not integer\n", x);
				return 1;
			}
		}
	}
	int n = atoi(argv[1]);
	int w = atoi(argv[2]);
	//int w = 3;
	if (n < 1 || n > 1000000000) {
		fprintf(stderr, "Agument n is not from interval <1, 10e9>\n");
		return 1;
	}
	if (w < 1 || w > 100) {
		fprintf(stderr, "Agument w is not from interval <1, 100>\n");
		return 1;
	}

	clock_t start_1t, end_1t, start_2t, end_2t;
	double total_1t, total_2t;


	HANDLE threads[w];
	DWORD thrdids[w];
	struct threadInfo ti[w];

	mutex = CreateMutex(NULL, FALSE, NULL);


	if (n % w != 0) {
		for (int i = 0; i < w - 1; i++) {
			(ti+i)->n = n / w;
			(ti+i)->first = (n / w) * i;
			(ti+i)->retval = 1.0;
			threads[i] = CreateThread(NULL, 0, thread, ti + i, 0, thrdids + i);
		}
		(ti+w-1)->n = (n / w) + (n % w);
		(ti+w-1)->first = (n / w) * (w - 1);
		(ti+w-1)->retval = 1.0;
		threads[w-1] = CreateThread(NULL, 0, thread, ti+w-1, 0, thrdids+w-1);

	}
	else {
		for (int i = 0; i < w; i++) {
			(ti + i)->n = n / w;
			(ti + i)->first = (n / w) * i;
			(ti + i)->retval = 0.0;
			threads[i] = CreateThread(NULL, 0, thread, ti + i, 0, thrdids + i);
		}
	}

	double wynik = 0.0;
	start_1t = clock();
	for (int i = 0; i < w; i++) {
		WaitForSingleObject(threads[i], INFINITE);
		printf("Thread %d completed: %.15f\n", i + 1, ti[i].retval);
		wynik += ti[i].retval;
		CloseHandle(threads[i]);
	}
	wynik = wynik * 4.0;
	end_1t = clock();


	double wynik2 = 0.0, temp;
	start_2t = clock();
	for (double i = 0; i <= n; i++) {
		temp = (double)pow(-1, i) / (double)(2 * i + 1);
		wynik2 += temp;
	}
	wynik2 = wynik2 * 4.0;
	end_2t = clock();


	total_1t = (double)(end_1t - start_1t) / CLOCKS_PER_SEC;
	total_2t = (double)(end_2t - start_2t) / CLOCKS_PER_SEC;

	fprintf(stdout, "Wynik w/Threads: %.15lf  time: %.5f sec\n", wynik, total_1t);
	fprintf(stdout, "Wynik wo/Threads: %.15lf  time: %.5f sec\n", wynik2, total_2t);

	return 0;
}
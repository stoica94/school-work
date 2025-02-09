#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

int NReps;
int printLevel;
int N;
int P;
int* v;
int *vQSort;
int *vNew;

pthread_barrier_t barrier;

int min(int a, int b) {
	return (a < b)? a : b;
}

void* threadFunction(void* var)
{
	int i;
	int thread_id = *(int*)var;
	int start = (int)ceil(N / P * thread_id);
	int end = min((int)ceil(N / P * (thread_id + 1)), N);
	// printf("start: %d, end: %d\n", start, end);

	int width, *aux;
	for (width = 1; width < N; width = 2 * width) {

		start = floor(start / (2 * width)) * 2 * width;
		end = floor(end / (2 * width)) * 2 * width;
		printf("ThreadID: %d. Width: %d, Start: %d, End: %d\n", thread_id, width, start, end);

		for (i = start; i < end; i = i + 2 * width) {
			merge(v, i, i + width, i + 2 * width, vNew);
		}
		pthread_barrier_wait(&barrier);
		if (thread_id == 0) {
			aux = v;
			v = vNew;
			vNew = aux;
		}
		pthread_barrier_wait(&barrier);
	}
}

void merge(int source[], int start, int mid, int end, int destination[]) {
	// DO NOT MODIFY
	int iA = start;
	int iB = mid;
	int i;

	for (i = start; i < end; i++)
	{
		if (end == iB || (iA < mid && source[iA] <= source[iB])) {
			destination[i] = source[iA];
			iA++;
		} else {
			destination[i] = source[iB];
			iB++;
		}
	}
}

void compareVectors(int * a, int * b) {
	// DO NOT MODIFY
	int i;
	for(i = 0; i < N; i++) {
		if(a[i]!=b[i]) {
			printf("Sorted incorrectly\n");
			return;
		}
	}
	printf("Sorted correctly\n");
}

void displayVector(int * v) {
	// DO NOT MODIFY
	int i;
	int displayWidth = 2 + log10(v[N-1]);
	for(i = 0; i < N; i++) {
		printf("%*i", displayWidth, v[i]);
		if(!((i+1) % 20))
			printf("\n");
	}
	printf("\n");
}

int cmp(const void *a, const void *b) {
	// DO NOT MODIFY
	int A = *(int*)a;
	int B = *(int*)b;
	return A-B;
}

void getArgs(int argc, char **argv)
{
	if(argc < 4) {
		printf("Not enough paramters: ./program N NReps printLevel\nprintLevel: 0=no, 1=some, 2=verbouse\n");
		exit(1);
	}
	N = atoi(argv[1]);
	P = atoi(argv[2]);
	printLevel = atoi(argv[3]);
	NReps = 1;
}

void init()
{
	int i;
	v = malloc(sizeof(int) * N);
	vQSort = malloc(sizeof(int) * N);
	vNew = malloc(sizeof(int) * N);
	if(v == NULL) {
		printf("malloc failed!");
		exit(1);
	}

	// generate the vector v with random values
	// DO NOT MODIFY
	srand(42);
	for(i = 0; i < N; i++)
		v[i] = rand()%N;
}

void printPartial()
{
	compareVectors(v, vQSort);
}

void printAll()
{
	displayVector(v);
	displayVector(vQSort);
	compareVectors(v, vQSort);
}

void print()
{
	if(printLevel == 0)
		return;
	else if(printLevel == 1)
		printPartial();
	else
		printAll();
}

int main(int argc, char *argv[])
{
	int i, j;
	getArgs(argc, argv);
	init();

	pthread_t tid[P];
	int thread_id[P];

	for(i = 0;i < P; i++)
		thread_id[i] = i;


	// make copy to check it against qsort
	// DO NOT MODIFY
	for(i = 0; i < N; i++)
		vQSort[i] = v[i];
	qsort(vQSort, N, sizeof(int), cmp);

	// int width, *aux;
	// for (width = 1; width < N; width = 2 * width) {
	// 	for (i = 0; i < N; i = i + 2 * width) {
	// 		merge(v, i, i + width, i + 2 * width, vNew);
	// 	}
	// 	aux = v;
	// 	v = vNew;
	// 	vNew = aux;
	// }

	pthread_barrier_init(&barrier, NULL, P);

	for(i = 0; i < P; i++) {
		pthread_create(&(tid[i]), NULL, threadFunction, &(thread_id[i]));
	}

	for(i = 0; i < P; i++) {
		pthread_join(tid[i], NULL);
	}

	pthread_barrier_destroy(&barrier);

	print();

	return 0;
}

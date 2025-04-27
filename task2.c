#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

int thread_c;
int **lr;
pthread_spinlock_t spin;

typedef struct{
    int id;
    int start;
    int end;
    int *arr;
    int targ;
} Data;

void* search(void* arg);

int main(){
    int n, target;
    printf("Введите длину массива: ");
    scanf("%d", &n);
    printf("Введите искомое число: ");
    scanf("%d", &target);

    int *mas = malloc(n * sizeof(n));
    printf("Заполните массив %d элементами:\n", n);
    for (int i = 0; i < n; i++)
	scanf("%d", &mas[i]);
    
    printf("Введите число потоков: ");
    scanf("%d", &thread_c);

    lr = malloc(thread_c * sizeof(int*));
    pthread_spin_init(&spin, 0);
    pthread_t thread[thread_c];

    Data data[thread_c];
    int part = n / thread_c;
    for (int i = 0; i < thread_c; i++){
	data[i] = (Data){
	    .id = i,
	    .start = i * part,
	    .end = (i == thread_c - 1 ? n : (i + 1) * part),
	    .arr = mas,
	    .targ = target
	};
	pthread_create(&thread[i], NULL, search, &data[i]);
    }

    int count = 0;

    for (int i = 0; i < thread_c; i++)
	pthread_join(thread[i], NULL);

    for(int i = thread_c - 1; i >= 0; i--){
	for (int j = thread_c - 1; j >= 0; j--){
	    if(lr[i][j] > -1){
		count++;
		printf("%d ", lr[i][j]);
	    }
        }
    }

    if (count == 0)
	printf("Искомое число отсутствует в массиве\n");
    else
	printf("\n");

    pthread_spin_destroy(&spin);
    free(lr);
    free(mas);
    return 0;
}

void* search(void* arg){
    Data *d = (Data*)arg;
    int c = 0;
    int tar = -1;
    int *ind = malloc(thread_c * sizeof(int));
    for (int i = 0; i < thread_c; i++){
        ind[i] = -1;
    }
    pthread_spin_lock(&spin);
    for (int i = d->start; i < d->end; i++){
        if (d->arr[i] == d->targ)
	    ind[c++] = i;
	else
	    ind[c++] = -1;
    }
    
    lr[d->id] = ind;
    pthread_spin_unlock(&spin);
    return NULL;
}

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int thread_c;
int *lr;
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

    lr = malloc(thread_c * sizeof(int));
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

    for (int i = 0; i < thread_c; i++)
	pthread_join(thread[i], NULL);

    int max_index = -1;
    for(int i = 0; i < thread_c; i++){
	if (lr[i] > max_index)
	    max_index = lr[i];
    }

    if (max_index != -1)
	printf("Последние вхождение %d на индексе %d\n", target, max_index);
    else
	printf("Искомое число отсутствует в массиве\n");

    pthread_spin_destroy(&spin);
    free(mas);
    free(lr);

    return 0;
}

void* search(void* arg){
    Data *d = (Data*)arg;
    int tar = -1;
    pthread_spin_lock(&spin);
    for (int i = d->end; i > d->start; i--){
	if (d->arr[i] == d->targ){
	    tar = i;
	    break;
	}
    }

    lr[d->id] = tar;
    pthread_spin_unlock(&spin);
    return NULL;
}

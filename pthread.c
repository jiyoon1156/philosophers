#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

pthread_mutex_t mutex;
int cnt = 0;

void *count(void *arg)
{
    int i;
    char *name = (char *)arg;

    //상호배재(mutual exclusion), 동기화 기법

    //mutex를 이용하여 임계 구역을 진입할 때 그 코드 구역을 잠가
    pthread_mutex_lock(&mutex);
    
    //========critical section==========
    //임계 영역 : 하나의 한 스레드만이 진입해야하는 특정 코드 구역, 공유 자원의 변경이 일어날 수 있음.
    cnt = 0; // cnt = 공유자원
    i = 0;
    while (i < 10)
    {
        printf("%s cnt : %d\n", name, cnt);
        cnt++;
        usleep(1);
        i++;
    }
    //========critical section==========

    //mutex를 이용하여 잠긴 코드 구역을 풀어 다음 스레드가 진입할 수 있도록 해 줌
    pthread_mutex_unlock(&mutex);
}
int main()
{
    pthread_t thread1;
    pthread_t thread2;

    //mutex 초기화
    pthread_mutex_init(&mutex, 0);

    pthread_create(&thread1, 0, count, (void *)"thread 1");
    pthread_create(&thread2, 0, count, (void *)"thread 2");

    pthread_join(thread1, 0);
    pthread_join(thread2, 0);

    pthread_mutex_destroy(&mutex);
}
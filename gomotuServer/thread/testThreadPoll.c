/*
 *	用c语言或者c++11的线程库来实现一个线程池的简单模型
 */
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

//首先测试pthread_cond_wait的作用

//线程的参数
struct threadArg
{
    int num;
};

pthread_mutex_t thmutex;
pthread_cond_t thcond;
int num = 0;

//线程处理函数
void* threadFun(void *arg)
{
    struct threadArg *argInfo = (struct threadArg *)arg;
    printf("start fun sleep 2: arg:%d,num:%d\n", argInfo->num, num++);
//    sleep(1);

    //线程池执行函数的模型，大的while循环，加上cond_wait循环
    while(1) {
        pthread_mutex_lock(&thmutex);
        while(num != 3) {
            pthread_cond_wait(&thcond, &thmutex);
        }
        pthread_mutex_unlock(&thmutex);

        printf("end fun sleep 2: arg:%d,num:%d\n", argInfo->num, num++);
    }
    free(argInfo);
    pthread_exit(NULL);
}

//创建多个线程
void create_lot_thread(int num)
{
    pthread_mutex_init(&thmutex, NULL);
    pthread_cond_init(&thcond, NULL);

    for(int i = 0;i < num; ++i)
    {
        pthread_t tid;
        pthread_attr_t tht;
        pthread_attr_init(&tht);
        struct threadArg *arg = (struct threadArg *)malloc(sizeof(struct threadArg));
        arg->num = i;
        pthread_attr_setdetachstate(&tht, PTHREAD_CREATE_DETACHED);
        pthread_create(&tid, NULL, threadFun, arg);
        pthread_attr_destroy(&tht);
    }
}

int main()
{
    create_lot_thread(5);
    int n = 0;

    //必须等待一会，不然直接就退出了
    while(1) {
        scanf("%d", &n);
        pthread_mutex_lock(&thmutex);
        num = n;
        pthread_mutex_unlock(&thmutex);
        pthread_cond_signal(&thcond);
    }

    sleep(10);
    pthread_cond_destroy(&thcond);
    pthread_mutex_destroy(&thmutex);

    return 0;
}


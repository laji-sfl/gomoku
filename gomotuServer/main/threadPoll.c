/*
 *	线程池的实现
 */

#include "threadPoll.h"
//#include "threadFun.h"

extern void threadCall(struct TaskNote *arg);

char initThreadPoll(int threadNum, struct ThPoll **thPoll)
{
    pthread_t tid;  //线程号
    pthread_attr_t tattr;//线程属性
    int i = 0;//循环变量

    //初始化线程池结构体
    *thPoll = (struct ThPoll *)malloc(sizeof(struct ThPoll));
    (*thPoll)->head = (struct TaskNote *)malloc(sizeof(struct TaskNote));//*的优先级低于->和.
    (*thPoll)->head->fd = -1;
    (*thPoll)->head->next = NULL;
    (*thPoll)->isEnd = '1';
    pthread_mutex_init(&(*thPoll)->thmutext, NULL);//初始化锁和条件变量
    pthread_cond_init(&(*thPoll)->thcond, NULL);

    //设置线程为detach
    pthread_attr_init(&tattr);
    pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);

    //创建threadNum个线程
    for(i = 0;i < threadNum; ++i) {
        pthread_create(&tid, &tattr, threadFun, *thPoll);
		printf("threadNum%d: %u , ", i, (unsigned int)tid);
    }
	printf("\n");

    //销毁不用的资源
    pthread_attr_destroy(&tattr);
}

char addTaskToList(int fd, int epollfd, struct ThPoll *thPoll)
{
    //线程池被销毁了，就不允许添加任务
    if(thPoll->isEnd == '0') {
        printf("线程已被销毁\n");
		set_log("thread poll addTaskToList 线程池已被销毁");
        return '1';
    }

    //创建一个任务节点加入链表
    struct TaskNote *newTask = (struct TaskNote *)malloc(sizeof(struct TaskNote));
    newTask->fd = fd;
	newTask->epollfd = epollfd;

    //需要加锁访问防止造成冲突，TODO:是否会出现死锁，或者饿死
    pthread_mutex_lock(&thPoll->thmutext);
    newTask->next = thPoll->head->next; //头插法
    thPoll->head->next = newTask;
    pthread_mutex_unlock(&thPoll->thmutext);

    //唤醒一个线程
//	printf("加入任务链表，唤醒一个线程:addTaskToList\n");
    pthread_cond_signal(&thPoll->thcond);
	return '0';
}

void *threadFun(void *arg)
{
    struct ThPoll *thpoll = (struct ThPoll *)arg;

    //当线程池存在就一直等待任务链表，然后执行任务
    while(thpoll->isEnd == '1') {
        struct TaskNote *task = NULL, *current = thpoll->head;

        //等待条件变量，和任务链表
        pthread_mutex_lock(&thpoll->thmutext);
        while(thpoll->head->next == NULL) {
            pthread_cond_wait(&thpoll->thcond, &thpoll->thmutext);
        }
        pthread_mutex_unlock(&thpoll->thmutext);

        //判断线程池是否销毁
        if(thpoll->isEnd == '0')
            break;

        //取出一个任务,取出尾节点
        pthread_mutex_lock(&thpoll->thmutext);
        while(current->next != NULL) {
            task = current;         //记录的是尾节点的前一个
            current = current->next;
        }
        current = task->next;   //删除尾节点
        task->next = NULL;
        pthread_mutex_unlock(&thpoll->thmutext);

        //执行任务
		printf("开始执行任务thread id:%u \n", (unsigned int)pthread_self());
        if(current != NULL)
            /*运行任务处理函数*/
			threadCall(current);

        free(current);  //执行完任务就释放任务节点
    }

    //退出线程
	printf("线程退出\n");
    pthread_exit(NULL);
}

void destroyThreadPoll(struct ThPoll *thPoll)
{
    //先将标志设定，这样就避免添加任务，而且一部分线程就会退出
    pthread_mutex_lock(&thPoll->thmutext);
    thPoll->isEnd = '0';
    pthread_mutex_unlock(&thPoll->thmutext);

    //释放任务链表
    struct TaskNote *current = thPoll->head;
    pthread_mutex_lock(&thPoll->thmutext);
    while(current->next != NULL) {
        thPoll->head->next = current->next->next;   //记录下下一个
        free(current->next);    //删除下一个
        current = thPoll->head; //重新使current等于头结点
    }
    thPoll->head = NULL;
    pthread_mutex_unlock(&thPoll->thmutext);

    //发送广播直到所有线程都退出,因为已经将任务链表清空，所以线程收到信号后都会退出
    pthread_cond_broadcast(&thPoll->thcond);

    //释放线程池结构
    sleep(1);//确保线程在这时已经全部退出，或者添加线程号到结构体中，检测还有几个号，不过这个也行
    free(thPoll->head);
    pthread_mutex_destroy(&thPoll->thmutext);
    pthread_cond_destroy(&thPoll->thcond);
    free(thPoll);
}

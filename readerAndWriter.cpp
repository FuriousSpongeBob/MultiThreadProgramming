#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
 
#define N_WRITER 3 //写者数目
#define N_READER 5 //读者数目
#define W_SLEEP  1000 //控制写频率
#define R_SLEEP  1000 //控制读频率
 
 
pthread_t wid[N_WRITER],rid[N_READER];
pthread_mutex_t writeLock = PTHREAD_MUTEX_INITIALIZER;//同一时间只能一个人写文件,互斥
pthread_mutex_t accessReaderCnt = PTHREAD_MUTEX_INITIALIZER;//同一时间只能有一个人访问readerCnt变量
int data = 0;
int readerCnt = 0;
void write()
{
	int rd = rand();
	printf("write %d\n",rd);
	data = rd;
}
void read()
{
	printf("read %d\n",data);
}
void *writer(void * in)
{
	for(int j=0;j<10;j++)
	{
		pthread_mutex_lock(&writeLock);//申请写锁
		write();
		pthread_mutex_unlock(&writeLock);//释放写锁
		usleep(W_SLEEP);//每1ms写一次
	}
	pthread_exit((void *) 0);
}
 
void *reader (void * in)
{
	for(int j=0;j<10;j++)
	{
		pthread_mutex_lock(&accessReaderCnt);//readerCnt上锁（同一时间只能有一个读线程访问readerCnt）
		readerCnt++;//读者数量++
		if(readerCnt == 1){
			pthread_mutex_lock(&writeLock);//如果readerCnt>=1,上写锁（只用上一次）
		}
		pthread_mutex_unlock(&accessReaderCnt);//readerCnt解锁
		
		read();
		
		pthread_mutex_lock(&accessReaderCnt);//readerCnt上锁
		readerCnt--;//读者数量--
		if(readerCnt == 0){
			pthread_mutex_unlock(&writeLock);//如果readerCnt==0,无人在读，解开写锁
		}
		pthread_mutex_unlock(&accessReaderCnt);//readerCnt解锁
		usleep(R_SLEEP);//每1ms读一次
	}
	pthread_exit((void *) 0);
}
 
int main()
{
	int i = 0;
	for(i = 0; i < N_READER; i++)
	{
		pthread_create(&wid[i],NULL,reader,NULL);
	}
	for(i = 0; i < N_WRITER; i++)
	{
		pthread_create(&rid[i],NULL,writer,NULL);
	}
	while(1){
		usleep(10000);
	}
	return 0;
}
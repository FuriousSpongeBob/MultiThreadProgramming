#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
using namespace std;
pthread_mutex_t m[5];

void* tfn(void *arg)
{
	int i = (int)(size_t)arg;
	int left=i,right=i+1;
	for(int j=0;j<10;j++){
		pthread_mutex_lock(&m[left]);//必须先拿到左筷子才能拿右筷子
		printf("哲学家%d拿到左筷子\n",i);
		int ret = pthread_mutex_trylock(&m[right]); 	
		if(ret != 0){
			//尝试拿右手筷子失败，把左手的筷子释放
			pthread_mutex_unlock(&m[left]);
			printf("哲学家%d尝试拿右筷子失败，把左手的筷子释放\n",i);
			continue;
		}
		printf("哲学家%d拿到右筷子\n",i);

		//拿到了两支筷子
		//吃面
		printf("哲学家%d正在吃面...\n",i);

		pthread_mutex_unlock(&m[left]);
		printf("哲学家%d放下左筷子\n",i);
		pthread_mutex_unlock(&m[right]);
		printf("哲学家%d放下右筷子\n",i);
	}
}

int main()
{
	pthread_t pth[5];
	//初始化五把锁
	for(int i = 0;i < 5;i++){
		pthread_mutex_init(&m[i],NULL);
	}
	//创建5个线程，相当于5个哲学家
	for(int i = 0;i < 5;i++){
		pthread_create(&pth[i],NULL,tfn,(void*)i);//NULL表示默认属性,tfn表示进程运行的函数,arg 是 start_routine 所需要的参数，是一个无类型指针.
	}
	//回收子线程
	for(int i = 0;i < 5;i++){
		pthread_join(pth[i],NULL);
	}
	//退出
	pthread_exit(NULL);
	
}



#include<iostream>
#include<mutex>
#include<semaphore.h>
#include<pthread.h>
using namespace std;

#define P_NUM 2 //生产者的数量
#define C_NUM 3 //消费者的数量
#define BUFFER_SIZE 2

//创建一个信号量，用来实现临界区的互斥
mutex m;

//创建生产者的信号量
sem_t *empty ;
//创建消费者的信号量
sem_t *full;

int _count = 0;//保存缓冲区中数据数

void *produce(void*){
    for(int i=0;i<10;i++){
        sem_wait(empty);//Ｐ操作，empty--，如果小于０，block线程
        //临界区上锁，保证同时只能有一个生产者访问
        m.lock();
        cout << "生产了第" << ++_count << "个产品！" << endl;
        m.unlock();//使用完临界区后解锁，让其他生产者使用
        sem_post(full);//Ｖ操作，full++，如果小于等于０，唤醒线程
    }
}

void* consume(void*){
    for(int i=0;i<10;i++){
        sem_wait(full);//Ｐ操作，full--，如果小于０，block线程
        m.lock(); //临界区上锁，保证同时只能有一个消费者访问
        cout << "消费了一个产品，还剩余" << --_count<< "个产品" << endl;
        m.unlock();//使用完临界区后解锁，让其他消费者使用
        sem_post(empty);//Ｖ操作，ｆｕｌｌ加１，如果小于等于０，唤醒线程
    }
}

int main(){
    //创建两个生产者
    pthread_t producers[P_NUM];
    //创建三个消费者
    pthread_t consumers[C_NUM];

    //初始化信号量
    srand((unsigned)time(NULL)); 
    int num1 = rand();
    int num2 = rand();
    const char *s1=to_string(num1).c_str();cout<<s1<<endl;
    const char *s2=to_string(num2).c_str();cout<<s2<<endl;
    empty=sem_open(s1, O_CREAT|O_EXCL, S_IRWXU, BUFFER_SIZE);
    full=sem_open(s2, O_CREAT|O_EXCL, S_IRWXU, 0);
    if(empty==SEM_FAILED||full==SEM_FAILED){
        cout << "sem init failed!" << endl;
         return -1;
    }
    for(int i = 0;i < P_NUM;i++){
        pthread_create(&producers[i],NULL,produce,NULL);
    }
    for(int i = 0;i < C_NUM;i++){
        pthread_create(&consumers[i],NULL,consume,NULL);
    }

    for(int i = 0;i < P_NUM;i++){
        pthread_join(producers[i],NULL);
    }

        for(int i = 0;i < C_NUM;i++){
        pthread_join(consumers[i],NULL);
    }
    return 0;
}

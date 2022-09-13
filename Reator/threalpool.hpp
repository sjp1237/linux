#pragma once
#include<iostream>
#include<vector>
#include<pthread.h>
#include<queue>
#define THREAD_NUM 5
#define POOL_NUM 1024
using namespace std;
template<class T>
class threadpool{
  private:
    queue<T*> pool;
    pthread_mutex_t lock;
    pthread_cond_t empty_cond;
    pthread_cond_t full_cond;
    int thread_num=THREAD_NUM;
    int pool_num=POOL_NUM;
  public:
    threadpool(){
      pthread_mutex_init(&lock,NULL);
      pthread_cond_init(&empty_cond,NULL);
      pthread_cond_init(&full_cond,NULL);
      thread_num=THREAD_NUM;
    }

    static void* routine(void* arg){
      pthread_detach(pthread_self());
      threadpool* tp=(threadpool*)arg;

      cout<<"thread running..."<<endl;
      while(1){
        tp->Lock();
        while(tp->Empty()){
          pthread_cond_wait(&tp->empty_cond,&tp->lock);
        }

        T* task=tp->pool.front();
        tp->pool.pop();
        tp->Unlock();
        tp->Signal(&tp->full_cond);
        task->Run();

      }
    }

    void Signal(pthread_cond_t* cond){
      pthread_cond_signal(cond);
    }

    bool Empty(){
      return pool.empty();
    }

    void Lock(){
      pthread_mutex_lock(&lock);
    }

    void Unlock(){
      pthread_mutex_unlock(&lock);
    }

    void Push(T* task){
      Lock();
      while(pool.size()==pool_num){
        pthread_cond_wait(&full_cond,&lock);
      }

      pool.push(task);
      Unlock();
      pthread_cond_signal(&empty_cond);
      cout<<"push success.."<<endl;
    }

    void threadpoolInit(){
      pthread_t pid;
      for(int i=0;i<thread_num;i++){
        pthread_create(&pid,NULL,routine,this);
      }
      cout<<"thread create success"<<endl;
    }

    ~threadpool(){
      pthread_cond_destroy(&empty_cond);
      pthread_cond_destroy(&full_cond);
      pthread_mutex_destroy(&lock);
    }
};

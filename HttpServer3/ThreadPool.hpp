#pragma once
#include"Log.hpp"
#include<pthread.h>
#include<queue>
#include"HttpProtocol.hpp"
#define NUM 3 

class Event_item;
class ThreadPool
{
  private:
    int num;
    std::queue<Event_item*> q;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    static ThreadPool* sigle_instance;
  private:
    static void* ThreadRoutine(void* arg)
    {
      LOG(INFO,"ThreadRoutine");
      pthread_detach(pthread_self());
      ThreadPool* thread=(ThreadPool*) arg;
      for(;;)
      {
        thread->Lock();
        while(thread->Size()==0)
        {
            thread->Wait();
        }
        Event_item* t;
        thread->PopTask(t);
        thread->UnLock();
        t->RecvRequest();
      }
    }

    size_t Size()
    {
      return q.size();
    }

    void Wait()
    {
      pthread_cond_wait(&cond,&mutex);
    }

    void SignalThread()
    {
      pthread_cond_signal(&cond);
    }

    ThreadPool(int _num=NUM):num(_num){
      pthread_cond_init(&cond,nullptr);
      pthread_mutex_init(&mutex,nullptr);
    }

    void PopTask(Event_item* out)
    {
      out=q.front();
      q.pop();
    }

    void Lock()
    {
      pthread_mutex_lock(&mutex);
    }

    void UnLock()
    {
      pthread_mutex_unlock(&mutex);
    }

    void InitThreadPool()
    {
      pthread_t tid;
      for(int i=0;i<num;i++)
      {
        pthread_create(&tid,nullptr,ThreadRoutine,this);
      }
    }
  public:
    static ThreadPool* GetThreadPool()
    {
      LOG(INFO,"GetThreadPool");
      static pthread_mutex_t _mutex=PTHREAD_MUTEX_INITIALIZER;
      if(sigle_instance==nullptr)
      {
        pthread_mutex_lock(&_mutex);
        if(sigle_instance==nullptr)
        {
          sigle_instance=new ThreadPool();
          sigle_instance->InitThreadPool();
        }
        pthread_mutex_unlock(&_mutex);
      }
      return sigle_instance;
    }


    void PushTask(Event_item* in)
    {
      Lock();
      q.push(in);
      UnLock();
      if(q.size()>0)
      {
        SignalThread();
      }
    }

    ~ThreadPool(){
      pthread_mutex_destroy(&mutex);
      pthread_cond_destroy(&cond);
    } 
};

ThreadPool* ThreadPool::sigle_instance=nullptr;

#pragma once
#include"server.hpp"
#include<string>
#include<unordered_map>
#include<vector>
#include<sys/epoll.h>
#include"threalpool.hpp"
    class Event_item;
   class EpollServer;
   namespace Task{
     class task;
   };
  typedef int (*callback_t)(Event_item* item);
#define NUM 1024
#define WAIT_NUM 32
    class Event_item{
  public:
    std::string outbuffer;
    std::string intbuffer;
    std::vector<std::string> taskset;
    int socket;
    callback_t recver;
    callback_t writer;
    callback_t errnoer; 
    EpollServer* R=nullptr;

    Event_item(callback_t _recver,callback_t _writer,callback_t _errnoer){
      recver=_recver;
      writer=_writer;
      errnoer=_errnoer;
    }
    Event_item(){}
  };
//namespace ep_server{ 
  class EpollServer{
    private:
      int listen_sock;//监听套接字
      int epfd;
      std::unordered_map<int,Event_item*> mp;
    public:
    threadpool<Task::task> _threadpool;
      EpollServer(){}

      void InitServer(){
        epfd=epoll_create(NUM);
        _threadpool.threadpoolInit();
        cout<<"epoll_create success"<<endl;
      }

      void Run(){
        while(1){
          struct epoll_event ep[32];//保存就绪事件
          int sz=epoll_wait(epfd,ep,32,1000);//sz是获取就绪事件的个数
          if(sz>0){
            for(int i=0;i<sz;i++){
              int fd=ep[i].data.fd;
              //错误事件就绪
              if((ep[i].events&EPOLLERR)&&mp[fd]->errnoer){
                  mp[fd]->errnoer(mp[fd]);                             
              }

              //读事件就绪
              if((ep[i].events&EPOLLIN)&&mp[fd]->recver){
                //cout<<"读事件就绪"<<endl;
                mp[fd]->recver(mp[fd]);
              }

              //写事件就绪              
              if(ep[i].events&EPOLLOUT){
           //     cout<<"iiiiiiii"<<endl;
              }

            
              if((ep[i].events&EPOLLOUT)&&!mp[fd]->outbuffer.empty())
              {
                cout<<"writer begin"<<endl;
                mp[fd]->writer(mp[fd]);
              }
            }
        }
      }
    }

      //删除等待事件
      int  Deletevent(int fd){        
        if(epoll_ctl(epfd,EPOLL_CTL_DEL,fd,NULL)<0){
          cout<<"delete event failing,fd :"<<fd<<endl;
          return -1;
        }
        mp.erase(fd);
        return 0;
      }

      //添加等待事件函数
      int  Addevent(int fd,uint64_t event,Event_item* item){
         struct epoll_event* _event=new struct epoll_event();
         _event->events=event;
         _event->data.fd=fd;
         if(epoll_ctl(epfd,EPOLL_CTL_ADD,fd,_event)<0){
           //添加事件失败
           cout<<"epoll_server.hpp 88:Add epoll fail,fd :"<<fd<<endl;
           return -1;
         }
         mp.insert(std::make_pair(fd,item));
         return 0;
      }      


      int Modevent(int fd,uint64_t event,Event_item* item){
         struct epoll_event* _event=new struct epoll_event();
         _event->events=event;
          _event->data.fd=fd;
          if(epoll_ctl(epfd,EPOLL_CTL_MOD,fd,_event)<0){
            cout<<"epoll mod error"<<endl;
            return -1;
          }
          return 0;
      }
  };
//}



namespace Task{
  class task{
    public:
    int x=1;
    int y=1;
    char op='+';
    int res=2;
    Event_item* item;
    task(int _x,int _y,char _op,Event_item* _item)
      :x(_x),y(_y),op(_op),item(_item)
  {}
    void Run(){
      switch(op){
        case '+':
         res=x+y; 
          break;
        case '*':
          res=x*y;
          break;
        case '-':
          res=x-y;
          break;
        case '/':
          res=x/y;
          break;
        default:
          break;
      }

     item->outbuffer+=to_string(x);
     item->outbuffer+=op;
     item->outbuffer+=to_string(y);
     item->outbuffer+="=";
     item->outbuffer+=to_string(res);
     item->outbuffer+="\n";
     cout<<"Task begin"<<endl;
     cout<<"Task end"<<endl;
     item->R->Modevent(item->socket,EPOLLET|EPOLLOUT|EPOLLIN,item);
    }
    ~task(){
      x=0;
      y=0;
      op=' ';
      res=0;
    }
  };
}

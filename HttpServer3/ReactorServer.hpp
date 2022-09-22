#pragma once
#include<sys/epoll.h>
#include<unordered_map>
#include<iostream>
#include"HttpProtocol.hpp"
#define EPNUM 5
using std::cout;
using std::endl;
#define WAIT_NUM 32
  class EpollServer{
    private:
      int listen_sock;//监听套接字
      int epfd;
      std::unordered_map<int,Event_item*> mp;
    public:
      EpollServer(){}
      void InitServer();

      void Run();
    
      //删除等待事件
      int  Deletevent(int fd);        

      //添加等待事件函数
      int  Addevent(int fd,uint64_t event,Event_item* item);
            
      int Modevent(int fd,uint64_t event,Event_item* item);
  };



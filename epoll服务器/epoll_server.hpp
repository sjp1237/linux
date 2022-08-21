#pragma once
#include"server.hpp"
#define NUM 1024
#define WAIT_NUM 32;
#include<sys/epoll.h>

namespace ep_server{

  class EpollServer{
    private:
      int port;
      int listen_sock;
      
      int epfd;
    public:
      EpollServer(int _port):port(_port){}

      void InitServer(){
        listen_sock=sjp::server::Socket(); 
        sjp::server::Bind(listen_sock,port);
        sjp::server::Listen(listen_sock);
        epfd=epoll_create(NUM);
      }


      void Run(){
       Addevent(listen_sock,EPOLLIN);
        while(1){
          struct epoll_event ep[32];
          int sz=epoll_wait(epfd,ep,32,1000);
          if(sz>0){
            for(int i=0;i<sz;i++){
              if(ep[i].events==EPOLLIN){
                //可读事件
                if(ep[i].data.fd==listen_sock){
                  struct sockaddr peer;
                  socklen_t len;
                  int fd=accept(listen_sock,&peer,&len);
                  if(fd>0){
                    Addevent(fd,EPOLLIN);
                  }
                }
                else{
                  char str[1024];
                  size_t sz=recv(ep[i].data.fd,(void*)str,1024,MSG_DONTWAIT);
                  if(sz>0){
                    str[sz]='\0';
                    cout<<str<<endl;
                  }else if(sz==0){

                  }
                  else{
                     //对端关闭
                     Deletevent(ep[i].data.fd);
                     close(ep[i].data.fd);
                  }
                }
              }
              else if(ep[i].events==EPOLLOUT){
                //可写事件
                
              }
              else{
                //其他事件

              }
            }
          }else if(sz==0){
            cout<<"without file fd"<<endl;
          }else{
            cout<<"epoll_wait failing"<<endl;
          }
        }
      }

      void Deletevent(int fd){        
        if(epoll_ctl(epfd,EPOLL_CTL_DEL,fd,NULL)<0){
          cout<<"delete event failing,fd :"<<fd<<endl;
        }
      }

      void Addevent(int fd,uint64_t event){
         struct epoll_event _event;
         _event.events=event;
         _event.data.fd=fd;
         if(epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&_event)<0){
           cout<<"Add epoll fail,fd :"<<fd<<endl;
         }
      }      
  };
}


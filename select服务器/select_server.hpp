#pragma once
#include"server.hpp"
using std::cout;
using std::endl;
#include<vector>
#include<unistd.h>

#define NUM 1024
namespace sjp_select{
  class Selectserver{
    private:
      int listen_sock;
      unsigned short int port;
    public:
      Selectserver(unsigned short int _port):port(_port){}
       void InitSelectServer(){
        listen_sock=sjp::server::Socket();
        sjp::server::Bind(listen_sock,port);
        sjp::server::Listen(listen_sock);
      }

      void Run(){
        //v是存储需要等待的文件描述符
        std::vector<int> v(NUM,-1);
         fd_set readset;
        while(1){
         struct timeval time={5,0};//每隔5s,timeout一次
         //将监听套接字设置进v的第一个
         v[0]=listen_sock;
         FD_ZERO(&readset);
         int maxfd=0;
         for(int i=0;i<1024;i++){
          if(v[i]!=-1){
            cout<<v[i]<<" ";
            //将需要等待的文件描述符设置进readset位图当中
            FD_SET(v[i],&readset);
            if(maxfd<v[i]){
              //记录最大的文件描述符
              maxfd=v[i];
            }
          }
         }
         cout<<endl;

          switch(select(maxfd+1,&readset,NULL,NULL,&time)){
            case 0:
              cout<<"time out"<<endl;
              break;
            case -1:
              cout<<"select error"<<endl;
              break;
            default:
              {
              //处理事件
              //扫描数组中的所有文件描述符,判断是否有读事件发生
              for(int i=0;i<1024;i++){
                if(v[i]==-1)
                  continue;

                if(FD_ISSET(v[i],&readset)) {
                  //有读事件发生
                  if(v[i]==listen_sock){
                    //v[i]是listen_sock,说明有新连接到来
                    //将新的sockfd设置进程v数组当中
                    struct sockaddr addr;
                     socklen_t len;
                    int fd=accept(listen_sock,&addr,&len);
                    Addfd(v,fd); 
                  }else{
                    //有其他任务，则读取数据到缓冲区当中
                     char str[1024];
                     int sz=recv(v[i],(void*)str,sizeof(str),0);
                     if(sz>0){
                     str[sz]='\0';
                     cout<<str<<endl;
                     }else if(sz==0){
                       cout<<"对端关闭"<<endl;
                       close(v[i]);
                       v[i]=-1;
                     }else{
                       cout<<"读取失败"<<endl;
                     }
                  }
                }
              }
              }
              break;
          }//switch
        }//while
      }


      void Addfd(std::vector<int>& v,int fd){
        for(int i=0;i<NUM;i++){
          if(v[i]==-1){
            v[i]=fd;
            break;
          }
        }
      }
  };
} 

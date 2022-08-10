#pragma once 
#include<iostream>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<unistd.h>
#include<string>
#include<stdlib.h>
#include<sys/wait.h>

class TcpServer
{
  private:
    int port;//端口号
    int listen_sock;//监听套接字描述符
  public:
    TcpServer(int _port=8081)
      :port(_port)
      ,listen_sock(-1)
    {

    }

    void InitServer()
    {
      listen_sock=socket(AF_INET,SOCK_STREAM,0);//创建套接字
      if(listen_sock<0)
      {
        std::cerr<<"socket fail"<<std::endl;
        return;
      }
      std::cout<<"socket success"<<std::endl;
      struct sockaddr_in s;
      memset(&s,'\0',sizeof(s));
      s.sin_family=AF_INET;
      s.sin_port=htons(port);
      s.sin_addr.s_addr=INADDR_ANY;
      //绑定套接字
      if(bind(listen_sock,(struct sockaddr*)&s,sizeof(s))<0)
      {
        std::cerr<<"bind failing"<<std::endl;
        return;
      }  
      //让套接字去监听连接
      if(listen(listen_sock,5)<0)
      {
        std::cerr<<"listen failing"<<std::endl;
      }
    }

    void Loop()
    {
      struct sockaddr_in sockaddr;
      for(;;)
      {
        socklen_t len=sizeof(sockaddr); 
        //接受监听套接字其中的一个连接，然后在创建新的套接字给这个连接
        //新的套接字进行数据通信
        int sockfd=accept(listen_sock,(struct sockaddr*)&sockaddr,&len);
        if(sockfd<0)
        {
          std::cerr<<"accept fail"<<std::endl;
          continue;
        } 
        std::string ip=inet_ntoa(sockaddr.sin_addr);
        int port=ntohs(sockaddr.sin_port);
        std::cout<<"get new link..["<<ip<<"]:"<<port<<std::endl;
        //对新的套接字进行服务
        pid_t id=fork();
        if(id==0)
        {
          //子进程
          close(listen_sock);
         if(fork()>0)
          {
             exit(0);
          }
         Server(sockfd,ip,port);
        }
        close(sockfd);
        waitpid(-1,nullptr,0);
      }         
    }

    void Server(int sock,std::string ip,int port)//服务功能
    {
      char buffer[1024];
      while(true) {
        ssize_t size=read(sock,buffer,sizeof(buffer)-1);
        if(size>0)
        {
          buffer[size]='\0';
          std::cout<<ip<<":"<<port<<"#"<<buffer<<std::endl;
          write(sock,buffer,size);
        }
        else if(size==0)
        {             
          std::cout<<ip<<":"<<port<<"close"<<std::endl;
          break;
        }
        else{
          std::cout<<"port error!!"<<std::endl;
          break;
        }
      }
      //新的套接字使用完后，需要关闭文件描述符，防止文件描述符泄漏
      close(sock);
    }
};

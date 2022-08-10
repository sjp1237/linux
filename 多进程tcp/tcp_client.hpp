#pragma once
#include<iostream>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<sys/types.h>
#include<string>
#include<sys/socket.h>
#include<unistd.h>

class TcpClient
{
  private:
    int ser_port;//服务器端口号
    std::string ser_ip;//服务器的ip地址
    int sockfd;//本地套接字描述符
  public:
    TcpClient(std::string _ser_ip,int _ser_port):
      ser_port(_ser_port)
      ,ser_ip(_ser_ip)
      ,sockfd(-1)
  {

  }

    void InitTcpClient()
    {
      sockfd=socket(AF_INET,SOCK_STREAM,0);//创建套接字
      if(sockfd<0)
      {
        std::cerr<<"socket failing"<<std::endl;
        return;
      }
    }

      void Start()
      {
          struct sockaddr_in peer;
          memset(&peer,'\0',sizeof(peer));
          peer.sin_family=AF_INET;
          peer.sin_port=htons(ser_port);
          peer.sin_addr.s_addr=inet_addr(ser_ip.c_str());
          //本地套接字去连接服务器
          if(connect(sockfd,(struct sockaddr*)&peer,sizeof(peer))==0)
          {
            //连接成功，请求服务
            std::cout<<"connect success..."<<std::endl;
            Request(sockfd);
          }
          else{
            //fail
            std::cout<<"connect fail"<<std::endl;
          }
      }

      //请求服务任务接口
      void Request(int sockfd)
      {
        std::string message;
        char buffer[1024];
        while(true)
        {
          std::cout<<"Please Enter#";
          std::cin>>message;
          //往套接字中输入数据
          write(sockfd,message.c_str(),message.size());

          ssize_t size=read(sockfd,buffer,sizeof(buffer));
          if(size>0)
          {
            buffer[size]='\0';
            std::cout<<"server # "<<buffer<<std::endl;
          }
        }
      }

      ~TcpClient()
      {
        if(sockfd>0)
          close(sockfd);
      }
};

























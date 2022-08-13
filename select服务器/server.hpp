#pragma once
#include<iostream>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#define LOG_NUM 5
using std::cout;
using std::endl;
namespace sjp{
  class server{
    public:
    server(){}
    ~server(){}

    static int Socket(){
       int sockfd=socket(AF_INET,SOCK_STREAM,0);
       return sockfd;
    }
    
    static bool Bind(int sockfd,unsigned short int port){
      struct sockaddr_in s;
      memset(&s,'\0',sizeof(s));
      s.sin_family=AF_INET;
      s.sin_port=htons(port);
      s.sin_addr.s_addr=0;
      if(bind(sockfd,(struct sockaddr*)&s,sizeof(s))<0){
        cout<<"bind error"<<endl;
        _exit(-1);
      }
      return true;

    }

    static bool Listen(int sockfd){
      int i=listen(sockfd,LOG_NUM);
      if(i==-1){
        cout<<"listen fail"<<endl;
        _exit(-2);
      }
      return true;
    }
  };
}

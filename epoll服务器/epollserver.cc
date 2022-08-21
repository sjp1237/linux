#include"epoll_server.hpp"
#include<stdlib.h>

void Usage(){
  cout<<"Usage Way: epollserver port"<<endl;
}
int main(int argc,char* argv[]){
  if(argc!=2){
    Usage();
  }

  int port= atoi(argv[1]);
  ep_server::EpollServer* es=new ep_server::EpollServer(port);
  es->InitServer();
  es->Run();
}

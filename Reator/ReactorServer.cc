#include"ReactorServer.hpp"
#include"App_interface.hpp"
#include<stdlib.h>

void Usage(){
  cout<<"Usage Way: epollserver port"<<endl;
}
int main(int argc,char* argv[]){
  if(argc!=2){
    Usage();
  }

  int port= atoi(argv[1]);
  //创建epoll服务器对象
  EpollServer* es=new EpollServer();
  es->InitServer();
  int listen_sock=sjp::server::Socket(); 
  SetNonBlocking(listen_sock);
  sjp::server::Bind(listen_sock,port);
  sjp::server::Listen(listen_sock);
  Event_item item(accepter,nullptr,nullptr);
  item.socket=listen_sock;
  item.R=es;
  es->Addevent(listen_sock,EPOLLIN|EPOLLET,&item);
  es->Run();
}

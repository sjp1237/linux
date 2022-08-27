#include"server.hpp"
#include<stdlib.h>
int main(int argc,char* argv[])
{
  if(argc!=2)
  {
    cout<<"Usage :"<<argv[0]<< "port"<<endl;
  }
  string ip="127.0.1";//127.0.1表示本地环回
  int port=atoi(argv[1]);
  server* s=new server(ip,port);
  s->initUdpServer();
  s->Start();
  return 0;


}

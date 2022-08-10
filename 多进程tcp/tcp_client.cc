#include"tcp_client.hpp"
using namespace std;
#include<stdlib.h>
// 启动客户端的方式：./tcp_client ip port


int main(int argv,char* argc[])
{
  if(argv!=3)
  {
    cout<<"Usage : ./tcp_client ip port"<<endl;
    exit(-1);
  }
  
  TcpClient* tc=new TcpClient(argc[1],atoi(argc[2]));
  tc->InitTcpClient();
  tc->Start();//开始运行服务端
  return 0;
}


#include"tcp_server.hpp"
using namespace std;
//启动服务器的方式：./tcp_server port
int main(int argv,char* argc[])
{
  if(argv!=2)
  {
    cout<<"Usage: tcp_server port"<<endl;
    exit(-1);
  }

  TcpServer* ts=new TcpServer(atoi(argc[1]));//创建服务器
  ts->InitServer();//初始化服务器
  ts->Loop();//启动服务器
  return 0;
}

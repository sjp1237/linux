#include"client.hpp"

//./client "127.0.0.1" 8081
int main(int argc,char* argv[])
{
  if(argc!=3)
  {
    cout<<"Usuage"<<"server_ip server_port"<<endl;

  }
  char* ip=argv[1];
  int port=atoi(argv[2]);
  client* c=new client(ip,port);
  c->InitClient();
  c->start();
  return 0;
}

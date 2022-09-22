#include"Httpserver.hpp"
using std::cout;
using std::endl;
int main(int argv,char* argc[]){
  if(argv!=2){
    cout<<"./httpserver port"<<endl;
  }

  int port=atoi(argc[1]);
  cout<<port<<endl;
  Server::HttpServer* ser=new Server::HttpServer(port);
  ser->InitServer();
  ser->Run();
}

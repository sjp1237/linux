#include"select_server.hpp"
#include<stdlib.h>


void Usage(){
  cout<<"./server port"<<endl;
}
int main(int argv,char* argc[]){
  if(argv<2){
    Usage();
  }
  int port=atoi(argc[1]);
  cout<<port<<endl;
  sjp_select::Selectserver _server(port);
  _server.InitSelectServer();
  _server.Run();
  return 0;
}

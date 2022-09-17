#pragma once
#include"TcpServer.hpp"
#include"HttpProtocol.hpp"
namespace Server{
  class HttpServer{
    private:
    int listen_sock;
    int port;
    TcpServer* server=nullptr;
    public: 
    HttpServer(int _port):port(_port){
    }

    void InitServer(){
       server=TcpServer::GetTcpServer(port);
      listen_sock=server->Sock();
    }

    void Run(){
      while(true){
        int fd=server->Accept();
        if(fd>0){
          Http::Event_item event(fd);
          event.RecvRequest();
          event.SendResponse();
        }
      }
    }
  };
}

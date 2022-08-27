#include<iostream>
#include<string>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
using namespace std;

class server
{
  private:
    int port;
    string ip;
    int fd;
  public:
    server(string _ip,int _port)
      :ip(_ip),port(_port)
    {

    }

    bool initUdpServer()
    {
        fd=socket(AF_INET,SOCK_DGRAM,0);
        if(fd<0)
        {
          cout<<"socket fail\n"<<endl;
          return false;
        }

        cout<<"socket success"<<endl;

        struct sockaddr_in s;
        memset(&s,'\0',sizeof(s));
        s.sin_family=AF_INET;
        s.sin_port=htons(port);
        s.sin_addr.s_addr=inet_addr(ip.c_str());
        
        if(bind(fd,(struct sockaddr*)&s,sizeof(s))<0)
        {
            cerr<<"bind error"<<endl;
            return false;
        }

        cout<<"bind success"<<endl;
        return true;
    }

    void Start()
    {
      for(;;)
      {
#define NUM 32
        char buf[NUM];
        struct sockaddr_in peer;
        socklen_t len=0;
        ssize_t sz=recvfrom(fd,(void*)buf,sizeof(buf)-1,0,(struct sockaddr*)&peer,&len);
        if(sz>0)
        {
           buf[sz]='\0';
           int _port=ntohs(peer.sin_port);
           string s=inet_ntoa(peer.sin_addr);
           cout<<s<<":"<< _port<<"#"<<buf<<endl;
        }
        else{
        cerr<<"recvfrom error"<<endl;
        }

      }
    }


};

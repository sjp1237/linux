#include<iostream>
#include<string>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
using namespace std;


class client
{
  private:
    int sockfd;
    int sev_port;
    string sev_ip;
  public:
    client(string _sev_ip,int _sev_port)
      :sev_port(_sev_port)
      ,sev_ip(_sev_ip)
    {

    }

    bool InitClient()
    {
      sockfd=socket(AF_INET,SOCK_DGRAM,0);
      if(sockfd<0)
      {
        cout<<"socket error"<<endl;
        return false;
      }
      return true;
    }

    void start()
    {
      struct sockaddr_in dest;
      memset(&dest,'0',sizeof(dest));
      dest.sin_family=AF_INET;
      dest.sin_port=htons(sev_port);
      dest.sin_addr.s_addr=inet_addr(sev_ip.c_str());
      string buf;
      for(;;)
      {
        cout<<"Please Enter#"<<endl;
        cin>>buf;
        sendto(sockfd,(void*)buf.c_str(),(size_t)buf.size(),0,(struct sockaddr*)&dest,sizeof(dest));
      }

    }



};

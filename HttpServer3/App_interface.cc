#include"App_interface.hpp"


bool SetNonBlocking(int fd){
  int flags=fcntl(fd,F_GETFL,0);
  fcntl(fd,F_SETFL,flags|O_NONBLOCK);
}

int accepter(Event_item* item){
  int listen_sock=item->socket;
  while(true){
    struct sockaddr peer;
    socklen_t len;
    int newfd=accept(listen_sock,&peer,&len);
    if(newfd>0){
      LOG(1,"accept success");
      SetNonBlocking(newfd);
      Event_item* newitem=new Event_item(newfd,recver,writer);
      newitem->R=item->R;
      newitem->socket=newfd;
      item->R->Addevent(newfd,EPOLLIN|EPOLLOUT|EPOLLET,newitem); 
    }
    else{
      //读取失败
      if(errno==EAGAIN||errno==EWOULDBLOCK){
        return 0;
      }
      else{
        //读取错误
        return -1;
      }
    }
    //cout<<"accepter end"<<endl;
  }
}

int recver(Event_item* item){
  item->RecvRequest();
  return 0;
}

int writer(Event_item* item){
  item->SendResponse();
  return 0;
}


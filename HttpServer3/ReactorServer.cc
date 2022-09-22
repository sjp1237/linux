#include"ReactorServer.hpp"

      void EpollServer::InitServer(){
        epfd=epoll_create(EPNUM);
        cout<<"epoll_create success"<<endl;
      }

      void EpollServer::Run(){
        while(1){
          struct epoll_event ep[32];//保存就绪事件
          int sz=epoll_wait(epfd,ep,32,1000);//sz是获取就绪事件的个数
          if(sz>0){
            for(int i=0;i<sz;i++){
              int fd=ep[i].data.fd;              
              //读事件就绪
              if((ep[i].events&EPOLLIN)){
                //cout<<"读事件就绪"<<endl;
                mp[fd]->RecvRequest();
              }

        
              if(ep[i].events&EPOLLOUT)
              {
                cout<<"writer begin"<<endl;
                mp[fd]->SendResponse();
              }
            }
        }
      }
    }

      int  EpollServer::Deletevent(int fd){        
        if(epoll_ctl(epfd,EPOLL_CTL_DEL,fd,NULL)<0){
          cout<<"delete event failing,fd :"<<fd<<endl;
          return -1;
        }
        mp.erase(fd);
        return 0;
      }



      int  EpollServer::Addevent(int fd,uint64_t event,Event_item* item){
         struct epoll_event* _event=new struct epoll_event();
         _event->events=event;
         _event->data.fd=fd;
         if(epoll_ctl(epfd,EPOLL_CTL_ADD,fd,_event)<0){
           //添加事件失败
           cout<<"epoll_server.hpp 88:Add epoll fail,fd :"<<fd<<endl;
           return -1;
         }
         mp.insert(std::make_pair(fd,item));
         return 0;
      }      


      int EpollServer::Modevent(int fd,uint64_t event,Event_item* item){
          struct epoll_event* _event=new struct epoll_event();
         _event->events=event;
          _event->data.fd=fd;
          if(epoll_ctl(epfd,EPOLL_CTL_MOD,fd,_event)<0){
            cout<<"epoll mod error"<<endl;
            return -1;
          }
          return 0;
      }


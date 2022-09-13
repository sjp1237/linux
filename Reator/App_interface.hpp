#pragma once
#include"server.hpp"
#include<unistd.h>
#include<vector>
#include<fcntl.h>
#include"tool.hpp"
#include"threalpool.hpp"
#include"ReactorServer.hpp"
//声明函数
Event_item * it=nullptr;
int ProductTask(std::vector<std::string>& taskset,threadpool<Task::task>& _threadpool,Event_item* item);

bool HandleTask(std::vector<Task::task*>& task_queue,std::string& outbuffer);
int recver(Event_item* item);
int writer(Event_item* item);

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
      SetNonBlocking(newfd);
      Event_item* newitem=new Event_item(recver,writer,nullptr);
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
  int fd=item->socket;
  cout<<"recver begin"<<endl;
  while(true){
    char buffer[1024];
    int sz=recv(fd,buffer,sizeof(buffer),MSG_DONTWAIT);
    if(sz>0){
      for(int i=0;i<sz;i++){
        if(buffer[i]!='\n')
         item->intbuffer+=buffer[i];
      }
    }else{
      if(errno==EAGAIN){
        //没有数据跳出循环
         break;
      }else{
        //出错
        return -1;
      }
    }
  }
  //item->intbuffer.pop_back();
  cout<<"inbuffe begin"<<endl;
  cout<<item->intbuffer<<endl;
  cout<<"inbuffe end"<<endl;
  std::vector<std::string>& taskset=item->taskset;
  while(!item->intbuffer.empty()){
    string tmp;
    bool flag=tool::CutString(item->intbuffer,std::string("x"),tmp);
    if(flag)
    taskset.push_back(tmp);
    else break;
  }

  cout<<"taskset begin"<<endl;
  for(auto& s:taskset){
    cout<<s<<endl;
  }
  cout<<"taskset end"<<endl;
  if(!taskset.empty()){
    ProductTask(taskset,item->R->_threadpool,item);
  }

  //cout<<item->outbuffer<<endl;
  return 0;
}

int writer(Event_item* item){
  int fd=item->socket;
  string& outbuffer=item->outbuffer;
  while(true){
   int sz=send(fd,outbuffer.c_str(),outbuffer.size(),MSG_DONTWAIT);
   if(sz>0){
     outbuffer=outbuffer.substr(sz);
     cout<<"outbuffer"<<outbuffer<<"endl"<<endl;
   }
   else if(sz==0){
     break;
   }else{
     cout<<"write error"<<endl;
     return -1;
   }
  
  }
  return 0;
}

int errnoer(Event_item* item){

}

int ProductTask(std::vector<std::string>& taskset,threadpool<Task::task>& _threadpool,Event_item* item){
  while(!taskset.empty())
  {
   // cout<<"ProductTask,begin:"<<endl;
    string s=taskset.back();
    taskset.pop_back();
   // cout<<s<<endl;
   // cout<<"ProductTask,endl:"<<endl;
    for(int i=0;i<s.size();i++){
      if(s[i]>='0'&&s[i]<='9'){
        continue;
      }else{
        //找到运算符
       // cout<<"begin "<<endl;
        char op=s[i];
        string x=s.substr(0,i);
        string y=s.substr(i+1);
        int _x=atoi(x.c_str());
        int _y=atoi(y.c_str());

        Task::task* newt=new Task::task(_x,_y,op,item);
        _threadpool.Push(newt);

        //Event_item* newitem=new Event_item(nullptr,writer,errnoer);
        //item->R->Addevent(item->socket,EPOLLOUT,newitem);
        //cout<<"task begin"<<endl;
        //cout<<"x:"<<newt->x<<endl;
        //cout<<"op:"<<newt->op<<endl;
        //cout<<"y:"<<newt->y<<endl;
        //cout<<"res:"<<newt->res<<endl;
        //cout<<"task endl"<<endl;
        break;
      }
    }
  }
}


bool HandleTask(std::vector<Task::task*>& task_queue,std::string& outbuffer){
  while(!task_queue.empty()){
    string s;
    Task::task* t=task_queue.back();
    task_queue.pop_back();
    s+=to_string(t->x);
    s+=t->op;
    s+=to_string(t->y);
    s+="=";
    s+=to_string(t->res);
    s+="\n";
    outbuffer+=s;
  } 
}

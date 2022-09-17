#pragma once
#include<cctype>
#include<stdlib.h>
#include<fcntl.h>
#include<string>
#include<sstream>
#include<iostream>
#include<unordered_map>
#include<vector>
#include"tool.hpp"
#include<algorithm>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/sendfile.h>
#define HOME_PAGE "index.html"
#define NOT_FOUND_PAGE "wwwroot/404.html"
#define NOT_FOUND "404"
#define OK "200"

namespace Http{
  //文件后缀所对应文件类型的映射
  static std::unordered_map<std::string,std::string> suffix_type{{"html","text/html"}};

  class Request{
    public:
      std::string request_line;
      std::string method;
      std::string uri;
      std::string version;
      std::string path;
      std::string parameter;
      int body_size;
      std::string content;
      std::vector<std::string> request_header;
      std::unordered_map<std::string,std::string> header_map;
      std::string blank;        
  };

  class Response{
    public:
      std::string response_line;
      std::string version;
      std::string code="200";
      std::string code_des;
      int fd;//网页文件的打开
      std::string suffix;//后缀
      int content_size;
      std::vector<std::string> response_header;
      std::string blank;
      std::string response_body;
  };


  class Event_item{
    private:
      bool cgi=false;//判断是否调用cgi程序
      Request request;//http请求
      Response response;//http响应
      int socket=-1;
      bool stop=false;
    private:
      bool Recvrequestline(){
        if(Tool::ReadLine(socket,request.request_line)){
          return true;
        }
        return false;
      }


      bool RecvreqestHeader(){
        while(true){
          std::string header;
          if(Tool::ReadLine(socket,header)){
            if(header=="\n"){
              request.blank=header;
              break;
            }
          else{
            //不是空行
            header.pop_back();
            request.request_header.push_back(header);
          }
        }
        else{
          //读取失败
          return false; 
        }
     }//while 
    return true;
    }

      bool AnalyRequestline(){
        auto& request_line=request.request_line;
        std::stringstream ss(request_line);
        ss>>request.method>>request.uri>>request.version;
        auto& method=request.method;
        transform(method.begin(),method.end(),method.begin(),::toupper);
        AnalyUri();
      }   

      bool AnalyUri(){
        std::string& uri=request.uri;
        //不管是Get方法还是Post方法,只要没找到"?", 则说明uri中一定没有参数
        if(uri.find("?")!=-1){
           //说明uri中带参数
          std::string seq="?";
          Tool::CuttingString(uri,request.path,request.parameter,seq);
        }else{
          //没有找到?,说明uri只有路径
           request.path=uri;    
        }
      }

      bool AnalyHeader(){
        for(auto& s:request.request_header){
          std::string s1,s2;
          std::string seq=": ";
          if(Tool::CuttingString(s,s1,s2,seq)){
           request.header_map.insert(make_pair(s1,s2));
          }
          else{
            return false;
          }
        }
        return true;
      }

      bool IsrecvContent(){
        if(request.method=="POST"){
          std::string sz=request.header_map["Content-Length"];
          request.body_size=stoi(sz);
          if(request.body_size>0)
            return true;
        }
        return false;
      }

      //读取请求正文
      bool RecvContent(){
        std::cout<<"recvContent .."<<std::endl;
        if(IsrecvContent()){
          int body_size=request.body_size;
          while(body_size--){
          char ch;
          int sz=recv(socket,&ch,1,0);
          if(sz>0){
            request.content+=ch;
          }
          else if(sz==0){
            return true;
          }else{
            stop=true; 
            return false;
          }
          }
        }
      }

      //判断为可执行文件
      void  AnalyFile(){
        std::string path=request.path;
        request.path="wwwroot";
        request.path+=path;
        if(request.path[request.path.size()-1]=='/'){
          request.path+=HOME_PAGE;
        }

        struct stat buf;
        if(stat(request.path.c_str(),&buf)==0){
          if(S_ISDIR(buf.st_mode)){
            //路径是一个目录，打开该目录下的首页信息
            request.path+="/";
            request.path+=HOME_PAGE;//HOME_PAGE: index.html
          }
          else if(buf.st_mode&S_IXUSR||buf.st_mode&S_IXGRP||buf.st_mode&S_IXOTH){
            //可执行文件
            cgi=true;
          }
          else{

          }
        }
        else{
          //找不到该文件
          //打开文件描述符
          response.code=NOT_FOUND;//404
          request.path=NOT_FOUND_PAGE;
          response.suffix="html";
          return;
        }
        //文件大小,也就是响应的正文的大小
        response.content_size=buf.st_size;
        //查找文件后缀
        int pos=request.path.rfind(".");
        if(pos>0){
          //文件后缀
          response.suffix=request.path.substr(pos+1);
        }else{
          //文件没有后缀,默认是html
          response.suffix="html";
        }
      }
       
      //通过状态码查找状态码描述
      //通过文件后缀来判断文件的类型
      //通过cgi是静态网页还是调用cgi程序
      void BuildResponseHeaer(){
        //构建Content-Type:        
        std::string content_type="Content-Type: ";
        content_type+=suffix_type[response.suffix];
        content_type+='\n';
        response.response_header.push_back(content_type);
        //构建Content-Length
        //非cgi的响应正文是文件的大小
        //cgi处理的响应正文是cgi程序处理的结果
        std::string content_size="Content-Length: ";
        content_size+=std::to_string(response.content_size);
        content_size+='\n';
        response.response_header.push_back(content_size);
      }


      bool CgiHandle(){
          std::cout<<"CgiHandle .."<<std::endl;
          //利用管道给cgi程序发送数据
          int pipe0[2];//父进程发送给子进程的管道
          int pipe1[2];//父进程接收子进程的管道
          if(pipe(pipe0)!=0){
            std::cout<<"pipe create fail"<<std::endl;
            return false;
          }
          if(pipe(pipe1)!=0){
            std::cout<<"pipe create fail"<<std::endl;
            return false;
          }

          if(fork()==0){
            //子进程
            close(pipe0[1]);//pipe0[0] 子进程用来读
            close(pipe1[0]);//pipe1[1] 子进程用来写
            dup2(pipe1[1],1);
            dup2(pipe0[0],0);
            //程序替换后,所有的数据和代码都会被清空,包括文件描述符，
            //只有环境变量不会替换掉
            //所以通过重定向文件描述符,则子进程可以通过cin向父进程输入数据
            //cout从管道中读取数据
            //子进程需要知道是什么方法来，从而判断是往管道还是环境变量中获取参数
            //子进程需要知道参数的大小，从而判断是否有读漏
            std::string method_env="METHOD=";
            method_env+=request.method;
            putenv((char*)method_env.c_str());
            if(request.method=="GET"){
              std::string parameter="PARAMETER=";
              parameter+=request.parameter;
              putenv((char*)parameter.c_str());
            }
            else if(request.method=="POST"){
              std::string body_size="Content-Length=";
              body_size+=std::to_string(request.body_size);
              putenv((char*)body_size.c_str());
          }

          execl(request.path.c_str(),request.path.c_str(),NULL);  
          }
          else{
            //父进程
            close(pipe0[0]);//pipe0[1]用来写数据给子进程
            close(pipe1[1]);//pipe1[0]读取子进程中的数据
            if(request.method=="POST"){
            auto& content=request.content;
            std::cout<<"content:"<<content<<std::endl;
             int sumsize=0;
             while(true){
              int sz=write(pipe0[1],content.c_str()+sumsize,content.size());
              if(sz>0){
                sumsize+=sz;
                if(sumsize==request.body_size)
                  break;
              }
              else{
                //写失败
                break;
              }
            }
          }
            while(true){
              char buf[128];
              int sz=read(pipe1[0],buf,sizeof(buf[128]));
              if(sz>0){
                //读取成功
                response.response_body+=buf;
              }else{
                //对端关闭
                break;
              }
            }//while
            response.content_size=response.response_body.size();
            std::cout<<"response_body:"<<response.response_body<<std::endl;
            close(pipe0[1]);
            close(pipe1[0]);
        }      
  }

      //非cgi网页的返回
      bool SendPage(){
        if(response.fd!=-1){
         int content_size=response.content_size; 
         std::cout<<content_size<<std::endl;
         off_t start=0;
         while(true){
           int size=sendfile(socket,response.fd,&start,content_size);
           if(size>0){
              if(start==content_size){
                break;
              }
           }
           else{
             std::cout<<start<<std::endl;
             std::cout<<response.fd<<std::endl;
             std::cout<<"sendpage error"<<std::endl;
             return false;
           }
         }
        }
      }


      void BuildReponseLine(){
        //构建响应行,版本 状态码 状态码描述
        response.response_line+="HTTP/1.1";
        response.response_line+=" ";
        response.response_line+=response.code;
        response.response_line+=" ";
        response.response_line+=response.code_des;
        response.response_line+="\n";
        std::cout<<response.response_line<<std::endl;
      }

      void Codehepler(){
        int code=atoi(response.code.c_str());
        switch(code){
          case 404:
            response.code_des="NOT FOUND";
            break;
          case 200:
            response.code_des="OK";
            break;
          default:
            break;
        }
        struct stat buf;
        if(stat(request.path.c_str(),&buf)<0){
          std::cout<<"Codehepler stat error"<<std::endl;
        }
        
        response.content_size=buf.st_size;
        response.fd=open(request.path.c_str(),O_RDONLY);
      }

      void SendResponseLine(){
        auto& response_line=response.response_line;
        int sum=0;
        while(true){
          int sz=send(socket,response_line.c_str()+sum,response_line.size(),0);
          if(sz>0){
            sum+=sz;
            if(sum==response_line.size())
              break;
          }
          else{
            //发送失败
          }
      }
    }

      void SendResponseHead(){
        auto& response_header=response.response_header;
        for(auto& str:response_header){
          int sum=0;
          while(true){
            int sz=send(socket,str.c_str()+sum,str.size(),0);
            if(sz>0){
              sum+=sz;
              if(sum==str.size())
                break;
            }
        }
      }
  }

      bool SendBlank(){
        int sz=send(socket,"\n",1,0);
        if(sz>0){
          return true;
        }
        else{

        }
      }

      //输出cgi处理的结果
      void SendCgiContent(){
        std::cout<<"Send CgiContent..."<<std::endl;
         int sum=0;
         auto& response_body=response.response_body;
         while(true){
           int sz=send(socket,response_body.c_str()+sum,response_body.size(),0);
           if(sz>0){
             sum+=sz;
             if(sz==response_body.size()){
               break;
             }
           }
         }
      }
      
      void BuildResponse(){
        //分析文件
        AnalyFile();
        //状态码描述
        Codehepler();

        //构建响应行
        BuildReponseLine();
        if(cgi){
          //cgi处理
          //构建cgi处理的响应正文,并知道响应正文的大小
         CgiHandle(); 
        }
        //构建响应报头
        BuildResponseHeaer();
      }
    public:
      Event_item(int _socket):socket(_socket){}
      void RecvRequest(){
        Recvrequestline();//读取请求行
        AnalyRequestline();//读取请求报头       
        RecvreqestHeader();
        AnalyHeader();
        if(IsrecvContent()){
          RecvContent(); 
        }  
        BuildResponse();
      }
 
      void SendResponse(){
        SendResponseLine();
        SendResponseHead();
        SendBlank();
        if(cgi){
          std::cout<<"cgi begin"<<std::endl;
          SendCgiContent(); 
        }
        else{
          SendPage();
        }
      }
  };
}

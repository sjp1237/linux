#pragma once
#include<cctype>
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
#define HOME_PAGE "index.html"
#define NOT_FOUND_PAGE "wwwroot/NOT_FOUND.html"
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
      std::string code="OK";
      std::string code_des;
      int fd;
      std::string suffix;//后缀
      int content_size;
      std::vector<std::string> response_header;
      std::string blank;
      std::string response_body;
  };



  class Event_item{
    private:
      bool cgi=false;
      Request request;
      Response response;
      int fd;
      bool stop=false;
    private:
      bool Recvrequestline(){
        if(Tool::ReadLine(fd,request.request_line)){
          return true;
        }
        return false;
      }


      bool RecvreqestHeader(){
        while(true){
          std::string header;
          if(Tool::ReadLine(fd,header)){
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
        if(IsrecvContent()){
          char ch;
          int sz=recv(fd,&ch,1,0);
          if(sz>0){
            request.content+=ch;
          }
          else if(sz==0){
            return true;
          }else{
            stop=true; 
          }
        }
      }

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
          response.code=NOT_FOUND;
          request.path=NOT_FOUND_PAGE;
          response.suffix="html";
          return;
        }

        //查找文件后缀
        response.content_size=buf.st_size;
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
      void BuildResponse(){
        AnalyFile();
        Codehepler();
        //构建Content-Type:
        std::string content_type="Content-Length: ";
        content_type+=suffix_type[response.suffix];
        response.response_header.push_back(content_type);



      }

      void BuildReponseLine(){
        //构建响应行,版本 状态码 状态码描述
        response.response_line+=request.version;
        response.response_line+=" ";
        response.response_line+=response.code;
        response.response_line+=" ";
        response.response_line+=response.code_des;
        response.response_line+="\n";
      }

      void Codehepler(){
        int code=stoi(response.code);
        switch(code){
          case 404:
            response.code_des="NOT FOUND";
            break;
          case 200:
            response.code_des="OK";
            break;
        }
      }

      bool Iscgi(){

      }

      bool NotCgiHandle(){

      }
      bool BuildResponseHeader(){
        
      }

    public:
      void RecvRequest(){
        Recvrequestline();
        AnalyRequestline();        
        RecvreqestHeader();
        AnalyHeader();
        if(IsrecvContent()){
          RecvContent(); 
        }  
      }


  };
}

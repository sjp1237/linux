#pragma once
#include<unordered_map>
#include"tool.hpp"
#include"comm.hpp"
#include"Log.hpp"
#define HOME_PAGE "index.html"
#define NOT_FOUND_PAGE "wwwroot/404.html"
#define NOT_FOUND "404"
#define OK "200"
#define BLANK "\r\n"

  //文件后缀所对应文件类型的映射
  static std::unordered_map<std::string,std::string> suffix_type{{"html","text/html"}};
  class Event_item;
  typedef int(* callback_t)(Event_item* );
  class EpollServer;

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
    public:
      int socket=-1;
      EpollServer* R; 
    private:
      bool cgi=false;//判断是否调用cgi程序
      Request request;//http请求
      Response response;//http响应
      bool stop=false;
      callback_t reader;
      callback_t writer;
    private:
      bool Recvrequestline();
      bool RecvreqestHeader();

      bool AnalyRequestline();

      bool AnalyUri();

      bool AnalyHeader();

      bool IsrecvContent();
      //读取请求正文
      bool RecvContent();

      //判断为可执行文件
      void  AnalyFile();
             
      //通过状态码查找状态码描述
      //通过文件后缀来判断文件的类型
      //通过cgi是静态网页还是调用cgi程序
      void BuildResponseHeaer();
     
      bool CgiHandle(); 
      //非cgi网页的返回
      bool SendPage();

      void BuildReponseLine();

      void Codehepler();

      void SendResponseLine();

      void SendResponseHead();

      bool SendBlank();

      //输出cgi处理的结果
      void SendCgiContent();
        
      void BuildResponse();
      
    public:
      Event_item(int _socket,callback_t _reader,callback_t _writer):socket(_socket){
        reader=_reader;
        writer=_writer;
      }
      void RecvRequest();//接送请求 
      void SendResponse();//发送响应
  };

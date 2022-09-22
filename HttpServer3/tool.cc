#include"tool.hpp"

bool Tool::ReadLine(int fd,std::string& outstring){
     while(true){
       char ch;
       if(recv(fd,&ch,1,0)>0){
         //读取成功
         if(ch!='\n'&&ch!='\r'){
           //读取一行
           outstring.push_back(ch);
         }else{
           //判断下一个是否为\r,如果是，则将它读取走
           if(ch=='\r'){  
            recv(fd,&ch,1,MSG_PEEK);
            if(ch=='\n'){
             recv(fd,&ch,1,0);
            }
           }
           outstring.push_back('\n');//+'\n'
           return true;
         }
       }
     //  else if(sz==0){
     //   //对端关闭
     //   return 0;
     //  }
     }
       return outstring.size();
  }

  bool Tool::CuttingString(std::string& s,std::string& s1,std::string& s2,std::string& seq){
    size_t pos=s.find(seq);
    if(pos!=std::string::npos){
      s1=s.substr(0,pos);
      s2=s.substr(pos+seq.size()-1,std::string::npos);
      return true;
    }else{
      return false;
    }
  }

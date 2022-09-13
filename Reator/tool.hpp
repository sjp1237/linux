#pragma once 
#include<iostream>
#include<string>

namespace tool{
  //将一个字符串以seq将其分开
  //aaaaxbbbbxcccc
  bool CutString(std::string& str,const std::string& seq,std::string& newstr){
    int pos=str.find(seq); 
    if(pos!=std::string::npos){
      //找到分隔符
      newstr=str.substr(0,pos);
      str=str.substr(pos+seq.size(),str.size()-pos);
      std::cout<<"newstr ： "<<newstr<<std::endl;
      std::cout<<"str ： "<<str<<std::endl;
      return true;
    }
    return false;//截取失败
  }
}

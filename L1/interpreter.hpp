#ifndef __INTERPRETER_HPP__
#define __INTERPRETER_HPP__

#include <string>
#include <functional>
#include <list>
#include <vector>
#include <iostream>
#include "utf8.hpp"

template <typename T>
inline void ConvertFromStr(std::string, T&) {
  static_assert(sizeof(T) == -1,"Invalid argument type");
}
template<> inline void ConvertFromStr<int>(std::string str, int& out){ out = std::stoi(str); }
template<> inline void ConvertFromStr<std::string>(std::string str, std::string& out){ out = str; }

class Interpreter{
public:
  void Run();
  void Prompt();
  void PrintHelp();
  
  void AddCommand(std::string command, std::string comment, std::function<void()> f);
  void AddCommandParse(std::string command, std::string comment, std::function<void(std::vector<std::string>)> f);
  void AddFlag(std::string name, std::string comment, bool* value_ref, bool default_val);


  template<typename... Args, typename Func>
  void AddCommandArgs(std::string command, std::string comment, Func&& f){
    AddCommandArgs__i(command,comment,std::function<void(Args...)>(f));
  }
  
  std::vector<std::string> GetWordList();
  
private:

  template<typename... Args>
  void AddCommandArgs__i(std::string command, std::string comment, std::function<void(Args...)> f){
    std::function<void(std::vector<std::string>, Args... args)> q = [f](std::vector<std::string>, Args... args){
	f(args...);
    };
    AddCommandArgs__c(command,comment,q);
  }
  
  template<typename First, typename... Args>
  inline void AddCommandArgs__c(std::string command, std::string comment, std::function<void(std::vector<std::string>, First, Args...)> f){
    AddCommandArgs__c(command,comment,std::function<void(std::vector<std::string>, Args...)>(
	    [f](std::vector<std::string> tail, Args... args){
	First x;
	if(tail.size() > 0){
	  ConvertFromStr<First>(tail.front(), x);
	  tail.erase(tail.begin());
	  //tail.pop_back();
	  f(tail,args...,x);
	}else{
	  std::cout << "Not enough args" << std::endl;
	}
	    })
    );
  }
  inline void AddCommandArgs__c(std::string command, std::string comment, std::function<void(std::vector<std::string>)> f){
    AddCommandParse(command,comment,f);
  }
  
  struct CommandEntry{
    std::string command;
    std::string comment;
    std::function<void(std::vector<std::string>)> f;
  };
  std::list<CommandEntry> command_list;

  void Left();
  void Right();
  void Up();
  void Down();
  
  utf8string buffer;
  utf8string tail;

  bool quit = false;
};

#endif //__INTERPRETER_HPP__

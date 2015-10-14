#include "interpreter.hpp"
#include "getch.hpp"
#include "utf8.hpp"
#include <cctype>
#include <iostream>

void Interpreter::AddCommand(std::string command_, std::string comment_, std::function<void()> f_){
  AddCommandParse(command_,comment_,[f_](auto){f_();});
}

void Interpreter::AddCommandParse(std::string command_, std::string comment_, std::function<void(std::vector<std::string>)> f_){
  CommandEntry ce;
  ce.command = command_;
  ce.comment = comment_;
  ce.f = f_;
  command_list.push_back(ce);
}

void Interpreter::AddFlag(std::string name, std::string comment, bool* value_ref, bool default_val){

}

void Interpreter::PrintHelp(){
  std::cout << "Available commands:" << std::endl;
  for(auto entry : command_list){
    std::cout << "  " << entry.command << " - " << entry.comment << std::endl;
  }
}

void Interpreter::Run(){
  quit = false;
  AddCommand("quit","Quits the interpreter",[this](){quit = true;});
  AddCommand("help","Prints this help",[this](){PrintHelp();});
  AddCommand("?","Prints this help",[this](){PrintHelp();});
  while(!quit){
    Prompt();
  }
}

void Interpreter::Left(){
  int n = buffer.length();
  if(n >= 1){
    utf8char last = buffer.split().back();
    buffer = buffer.substr(0,n-1);
    tail = last + tail;
  }
}
void Interpreter::Right(){
  int n = tail.length();
  if(n >= 1){
    utf8char last = tail.split().front();
    tail = tail.substr(1,n-1);
    buffer = buffer + last;
  }
}
void Interpreter::Down(){

}
void Interpreter::Up(){

}

void Interpreter::Prompt(){
    
  buffer = "";
  tail = "";
  while(1){
    std::cout << (char)27 << "[2K" << std::flush;
    std::cout << "\r";
    std::cout << " > " << buffer << tail;
    std::cout << "\r";
    std::cout << " > " << buffer;
    std::cout << std::flush;

    utf8char c = UTF8::getch();
    if(c.length() == 0){
      
    }else if(UTF8::isgraph(c)){
      buffer += c;
    }else if(c[0] == '\t'){
      // autocomplete
      
    }else if(c[0] == '\b' || c[0] == 127){
      // backspace
      if(buffer.length() >= 1)
	buffer = buffer.substr(0,buffer.length()-1);
      
    }else if(c[0] == ' '){
      // space
      buffer += c;
    }else if(c[0] == '\n' || c[0] == '\r'){
      std::cout << '\n' << std::flush;
      // execute

      auto v = GetWordList();
      if(v.size() < 1) return; // empty command
      std::list<CommandEntry> matches;
      std::string query = v[0];
      int n = query.length();
      for(CommandEntry ce : command_list){
	if(ce.command.substr(0, n) == query)
	  matches.push_back(ce);
      }
      if(matches.size() == 0){
	std::cout << "No match for command: '" << query << "'" << std::endl;
	return;
      }else if(matches.size() >=2){
	std::cout << "Command '" << query << "' is ambiguous" << std::endl;
	return;
      }

      // Only one match
      CommandEntry match = matches.front();
      v.erase(v.begin());
      match.f(v);
      
      return;
    }else if(c[0] == 4){
      // ctrl+D
      std::cout << (char)27 << "[2K" << "\r > quit" << std::endl;
      quit = true;
      return;
    }
    else if(c[0] == 2) Left(); // C-B
    else if(c[0] == 6) Right(); // C-F
    else if(c[0] == 16) Up(); // C-P
    else if(c[0] == 14) Down(); // C-N
    else if(c[0] == 27){
      // escape
      char c2 = getch();
      if(c2 == '['){
	char c3 = getch();
	if(c3 == 'A') Up();
	else if(c3 == 'B') Down();
        else if(c3 == 'C') Right();
        else if(c3 == 'D') Left();
      };
    }else{
      std::cout << "Unrecognized character " << c << "!" << std::endl;
    }
  }
}

std::vector<std::string> Interpreter::GetWordList(){
  auto v = (buffer+tail).words();
  std::vector<std::string> q;
  for(auto s : v) q.push_back(s);
  return q;
}

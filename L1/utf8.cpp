#include "utf8.hpp"
#include "getch.hpp"
#include <iostream>

std::string extra_chars[] = {u8"ą", u8"ę", u8"ł", u8"ó", u8"ś", u8"ć", u8"ń", u8"ż", u8"ź",
			     u8"Ą", u8"Ę", u8"Ł", u8"Ó", u8"Ś", u8"Ć", u8"Ń", u8"Ż", u8"Ź"};

utf8string utf8string::substr(int start, int length) const
{
  int i,ix,j,realstart,reallength;
  if (length==0) return "";
  if (start<0 || length <0)
    {
      //find j=utf8_strlen(str);
      for(j=0,i=0,ix=word.length(); i<ix; i+=1, j++)
	{
	  unsigned char c= word[i];
	  if      (c>=0   && c<=127) i+=0;
	  else if (c>=192 && c<=223) i+=1;
	  else if (c>=224 && c<=239) i+=2;
	  else if (c>=240 && c<=247) i+=3;
	  else if (c>=248 && c<=255) return "";//invalid utf8
	}
      if (length !=INT_MAX && j+length-start<=0) return "";
      if (start  < 0 ) start+=j;
      if (length < 0 ) length=j+length-start;
    }

  j=0,realstart=0,reallength=0;
  for(i=0,ix=word.length(); i<ix; i+=1, j++)
    {
      if (j==start) { realstart=i; }
      if (j>=start && (length==INT_MAX || j<=start+length)) { reallength=i-realstart; }
      unsigned char c= word[i];
      if      (c>=0   && c<=127) i+=0;
      else if (c>=192 && c<=223) i+=1;
      else if (c>=224 && c<=239) i+=2;
      else if (c>=240 && c<=247) i+=3;
      else if (c>=248 && c<=255) return "";//invalid utf8
    }
  if (j==start) { realstart=i; }
  if (j>=start && (length==INT_MAX || j<=start+length)) { reallength=i-realstart; }

  return word.substr(realstart,reallength);
}

utf8char UTF8::getch(){
  std::string res;
  int i = 0;
  char c = ::getch();
  res += c;
  if      (c>=0   && c<=127) i+=0;
  else if ((c & 0xE0) == 0xC0) i+=1;
  else if ((c & 0xF0) == 0xE0) i+=2;
  else if ((c & 0xF8) == 0xF0) i+=3;
  //else if (($c & 0xFC) == 0xF8) i+=4; // 111110bb //byte 5, unnecessary in 4 byte UTF-8
  //else if (($c & 0xFE) == 0xFC) i+=5; // 1111110b //byte 6, unnecessary in 4 byte UTF-8
  else return "";//invalid utf8
  while(i--){
    c = ::getch();
    res += c;
  }
  return res;
}

bool UTF8::isgraph(utf8char c){
  if(c.length() == 0) return false;
  if(c[0] >= 0 && c[0] <= 127) return ::isgraph(c[0]);
  for(const std::string& s : extra_chars){
    if(s == c) return true;
  }
  //return false;
  return true;
}

int utf8string::length() const
{
  int c,i,ix,q;
  for (q=0, i=0, ix=word.length(); i < ix; i++, q++)
    {
      c = (unsigned char) word[i];
      if      (c>=0   && c<=127) i+=0;
      else if ((c & 0xE0) == 0xC0) i+=1;
      else if ((c & 0xF0) == 0xE0) i+=2;
      else if ((c & 0xF8) == 0xF0) i+=3;
      //else if (($c & 0xFC) == 0xF8) i+=4; // 111110bb //byte 5, unnecessary in 4 byte UTF-8
      //else if (($c & 0xFE) == 0xFC) i+=5; // 1111110b //byte 6, unnecessary in 4 byte UTF-8
      else return 0;//invalid utf8
    }
  return q;
}

std::vector<utf8char> utf8string::split() const {
  std::vector<utf8char> res;
  int c,i,ix,q;
  for (i=0, ix=word.length(); i < ix; )
    {
      c = (unsigned char) word[i];
      int l = 0;
      if      (c>=0   && c<=127) l=1;
      else if ((c & 0xE0) == 0xC0) l=2;
      else if ((c & 0xF0) == 0xE0) l=3;
      else if ((c & 0xF8) == 0xF0) l=4;
      //else if (($c & 0xFC) == 0xF8) l=4; // 111110bb //byte 5, unnecessary in 4 byte UTF-8
      //else if (($c & 0xFE) == 0xFC) l=5; // 1111110b //byte 6, unnecessary in 4 byte UTF-8
      else return res;//invalid utf8
      res.push_back(word.substr(i,l));
      i+= l;
    }
  return res; 
}

std::string UTF8::uppercase_first(const std::string& str){
  std::string head;
  unsigned char c = str[0];
  int l;
  if      (c>=0   && c<=127) l=1;
  else if ((c & 0xE0) == 0xC0) l=2;
  else if ((c & 0xF0) == 0xE0) l=3;
  else if ((c & 0xF8) == 0xF0) l=4;
  //else if (($c & 0xFC) == 0xF8) l=4; // 111110bb //byte 5, unnecessary in 4 byte UTF-8
  //else if (($c & 0xFE) == 0xFC) l=5; // 1111110b //byte 6, unnecessary in 4 byte UTF-8
  else return str;//invalid utf8
  if(l == 1){
    std::string result = str;
    result[0] = ::toupper(result[0]);
    return result;
  }
  std::string first = str.substr(0,l);
  std::string tail = str.substr(l,std::string::npos);
  for(int i = 0; i < 9; i++){
    std::string ch = extra_chars[i];
    if(ch == first){
      // std::cout << "Substituting " << first << " with " << extra_chars[i+9] << std::endl;
      first = extra_chars[i+9];
      return first + tail;
    }
  }
  return str;
}

std::vector<utf8string> utf8string::words() const{
  std::vector<utf8string> res;
  std::utf8string curr;
  for(std::string c : split()){
    if(UTF8::isgraph(c)) curr+=c;
    else if(c.length() == 0) {}
    else if(isspace(c[0]) && curr != ""){
      res.push_back(curr);
      curr = "";
    }
  }
  if(curr != "") res.push_back(curr);
  return res;
}

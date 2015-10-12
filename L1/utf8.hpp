#ifndef __UTF8_HPP__
#define __UTF8_HPP__

#include <string>
#include <climits>
#include <vector>
#include <iostream>

typedef std::string utf8char;

class utf8string{
protected:
  std::string word;  
public:
  utf8string() : word() {}
  utf8string(std::string w) : word(w) {}
  utf8string(const char* ch) : word(ch) {}
  operator std::string() const {return word;}
  std::string str() const{return word;}
  bool operator==(const utf8string& other) const {return word == other.word;}
  bool operator!=(const utf8string& other) const {return word != other.word;}
  utf8string& operator+=(const utf8char& c) {word += c; return *this;}
  utf8string& operator+=(const utf8string& s) {word += s.str(); return *this;}
  utf8string operator+(const char* s) const {return utf8string(word + s);}
  utf8string operator+(const utf8char& c) const {return utf8string(word + c);}
  utf8string operator+(const utf8string& s) const {return utf8string(word + s.str());}
  utf8string substr(int start, int leng = INT_MAX) const;
  int length() const;
  std::vector<utf8char> split() const;
};

inline utf8string operator+(utf8char c, utf8string s) {
  return utf8string(c + s.str());
}

inline std::ostream& operator<<(std::ostream& stream, const utf8string& str){
  stream << str.str();
  return stream;
}

class UTF8{
public:
  static utf8char getch();
  static bool isgraph(utf8char c);
  static std::string uppercase_first(const std::string& str);
};

#endif // __UTF8_HPP__

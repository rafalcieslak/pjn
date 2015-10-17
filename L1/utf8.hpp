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
  explicit utf8string(char c) : word(1, c) {}
  utf8string(std::string w) : word(w) {}
  utf8string(const char* ch) : word(ch) {}
  utf8string(const utf8string& other) : word(other.word) {}
  utf8string(utf8string&& other)  : word(std::move(other.word)) {}
  operator std::string() const {return word;}
  std::string str() const {return word;}
  utf8string& operator=(const utf8string& other){word = other.word; return *this;}
  utf8string& operator=(utf8string&& other){ std::swap(word,other.word); return *this;}
  bool operator==(const utf8string& other) const {return word == other.word;}
  bool operator!=(const utf8string& other) const {return word != other.word;}
  utf8string& operator+=(const utf8char& c) {word += c; return *this;}
  utf8string& operator+=(const utf8string& s) {word += s.str(); return *this;}
  utf8string operator+(const char* s) const {return utf8string(word + s);}
  utf8string operator+(const utf8char& c) const {return utf8string(word + c);}
  utf8string operator+(const utf8string& s) const {return utf8string(word + s.str());}
  bool operator<(const utf8string& other) const {return word < other.word;}
  utf8string substr(int start, int leng = INT_MAX) const;
  const utf8char operator[](int n) const;
  int length() const;
  std::vector<utf8char> split() const;
  std::vector<utf8string> words() const;
  bool isSentenceEnding() const;
};

namespace std{
template<>
struct hash<utf8string>{
  std::size_t operator()(const utf8string& str) const{
    hash<std::string> hs;
    return hs(str);
  }
};

}

inline utf8string operator+(utf8char c, utf8string s) {
  return utf8string(c + s.str());
}

inline std::ostream& operator<<(std::ostream& stream, const utf8string& str){
  stream << str.str();
  return stream;
}
inline std::istream& operator>>(std::istream& stream, utf8string& str){
  std::string q;
  stream >> q;
  str = q;
  return stream;
}

class UTF8{
public:
  static utf8char getch();
  static bool isgraph(utf8char c);
  static std::string uppercase_first(const std::string& str);
};

#endif // __UTF8_HPP__

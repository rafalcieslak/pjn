#ifndef __UTF8_HPP__
#define __UTF8_HPP__

#include <string>
#include <climits>
#include <vector>

class UTF8{
public:
  static std::string getch();
  static std::string substr(const std::string& str, int start, int leng = INT_MAX);
  static bool isgraph(std::string c);
  static int strlen(const std::string& str);
  static std::vector<std::string> split(const std::string& str);
  static std::string uppercase_first(const std::string& str);
};

#endif // __UTF8_HPP__

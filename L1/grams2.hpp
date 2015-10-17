#ifndef __GRAMS2_HPP__
#define __GRAMS2_HPP__

#include <list>
#include <array>
#include "utf8.hpp"

class Words2{
public:
  struct Pattern{
    enum PatternType{
      ANY,
      SUFFIX,
      PREFIX,
      FULL
    };
    Pattern(const utf8string str){
      if(str == "" || str == "*"){pt = ANY; return;}
      if(str[0] == "*"){pt = SUFFIX; text = str.substr(1); return;}
      if(str[str.length()-1] == "*"){pt = PREFIX; text = str.substr(0,-1); return;}
      else{pt = FULL; text = str; return;}
    }
    Pattern(PatternType type, utf8string str = "")
      : pt(type), text(str) {}
    Pattern(const char* str) : Pattern(utf8string(str)) {}

    bool Matches(utf8string str) const{
      if(pt == ANY) return true;
      if(pt == FULL) return str == text;
      if(pt == PREFIX) return str.substr(0,text.length()) == text;
      if(pt == SUFFIX) return str.substr(-text.length()) == text;
      return false;
    }
    PatternType pt;
    utf8string text;

  };
  template <unsigned int N>
  struct QueryResults{
    std::list<std::pair<unsigned int, std::array<utf8string,N>>> list;
    unsigned int total = 0;
    std::array<utf8string,N> GetRandom(bool proportional = true) const{
      if(total == 0) return std::array<utf8string,N>();
      if(proportional){
	int q = rand()%total;
	for(const auto& pair : list){
	  q -= pair.first;
	  if(q <= 0) return pair.second;
	}
      }else{
	int q = rand()%list.size();
	for(const auto& pair : list){
	  q -= 1;
	  if(q < 0) return pair.second;
	}
      }
      return std::array<utf8string,N>();
    }
    void Print() const;
  };

  static QueryResults<1> Query1Grams(Pattern p1, unsigned int limit = 100000, unsigned int min_occ = 2);
  static QueryResults<2> Query2Grams(Pattern p1, Pattern p2, unsigned int limit = 100000, unsigned int min_occ = 3);
  static QueryResults<3> Query3Grams(Pattern p1, Pattern p2, Pattern p3, unsigned int limit = 100000, unsigned int min_occ = 2);
};


template <unsigned int N>
inline std::string JoinArray(std::array<utf8string,N> arr, utf8char ch){
  std::string res;
  for(unsigned int n = 0; n < N; n++) res += ((n>=0)?ch:"") + arr[n];
  return res;
}

template <unsigned int N>
inline void Words2::QueryResults<N>::Print() const{
  std::cout << "Query results, total = " << total << std::endl;
  for(auto pair = list.rbegin(); pair != list.rend(); pair++){
    std::cout << pair->first << "\t" << JoinArray<N>(pair->second,utf8char(" ")) << std::endl;
  }
}


#endif // __GRAMS2_HPP__

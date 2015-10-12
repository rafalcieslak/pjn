#ifndef __GRAMS_HPP__
#define __GRAMS_HPP__

#include <list>
#include <string>
#include <unordered_map>
#include <vector>
#include "utf8.hpp"

namespace std{
template <typename T, typename U>
struct hash<std::pair<T,U> > {
  size_t operator()(std::pair<T,U> p) const {
    return std::hash<T>()(p.first)*37 + std::hash<U>()(p.second);
  }
};
}

struct MWord : public utf8string{
  MWord() : utf8string() {}
  MWord(const std::string& str) : utf8string(str) {}
  MWord(const char* str) : utf8string(str) {}
  bool IsBeginning() const;
  bool IsEnding() const;
};


class Words{
public:

  void Init();

  // This structure takes the ownership of MWord pointers
  std::unordered_map<std::string, const MWord *> words;
  
  std::vector<std::pair<int,const MWord*>> grams1;
  int total_1grams_amt = 0;
  MWord GetRandom1Gram(bool proportional = true);

  int total_sentence_begs = 0;
  std::vector<std::pair<int,const MWord*>> sentence_begs;
  MWord GetRandomSentenceBeg(bool proportional = true);

  struct ContList{
    ContList(){}
    int total = 0;
    std::vector<std::pair<int, const MWord*>> list;
    MWord GetRandom(bool proportional = true);
  };

  std::unordered_map<std::string, ContList> gram2Conts;
  std::unordered_map<std::pair<std::string, std::string>, ContList> gram3Conts;
};

#endif // __GRAMS_HPP__

#include "grams.hpp"
#include "utf8.hpp"
#include <fstream>
#include <iostream>
#include <cstdlib>

bool MWord::IsBeginning() const{
  if(word.length() == 0) return false;
  std::string firstchar = UTF8::substr(word,0,1);
  if(firstchar.length() == 1 && !isalnum(firstchar[0])) return false;
  // if(firstchar.length() == 2){
  //   std::cout << "Oh, what a fancy character: " << firstchar << std::endl;
  // }
  return true;
}

bool MWord::IsEnding() const{
  char c = word.back();
  if(c == '?' || c == '!' || c == '.') return true;
  return false;
}

void Words::Init(){
  {
    std::ifstream file("1grams");
    if(!file){
      std::cout << "Failed to open file: 1grams" << std::endl;
      return;
    }
    std::cout << "Processing file 1grams" << std::endl;
    int n;
    std::string word;
    while(file >> n >> word){
      MWord* mw = new MWord(word);
      words[word] = mw;
      grams1.push_back({n, mw});
      total_1grams_amt += n;
    }
    file.close();

    std::cout << "Loaded " << grams1.size() << " 1grams." << std::endl;

    for(const auto& pair : grams1){
      if(pair.second->IsBeginning()){
	sentence_begs.push_back({pair.first,pair.second});
	total_sentence_begs += pair.first;
      }
    }

    std::cout << "Found " << sentence_begs.size() << " possible sentence beginnings." << std::endl;
  }
  {
    
    std::fstream file("2grams");
    if(!file){
      std::cout << "Failed to open file: 2grams" << std::endl;
      return;
    }
    std::cout << "Processing file 2grams" << std::endl;
    int count = 0;
    int n;
    std::string word1, word2;
    while(file >> n >> word1 >> word2){
      count++;
      const MWord* mw1 = words[word1];
      const MWord* mw2 = words[word2];
      if(mw1 == nullptr || mw2 == nullptr) // word not considered as a 1gram
	continue;

      auto it = gram2Conts.find(mw1);
      if(it == gram2Conts.end()){
	gram2Conts[mw1] = ContList();
	it = gram2Conts.find(mw1);
      }
      it->second.total += n;
      it->second.list.push_back({n,mw2});
    }
    file.close();
    std::cout << "Loaded " << count << " 2grams." << std::endl;
    
  }
  {
    
    std::fstream file("3grams");
    if(!file){
      std::cout << "Failed to open file: 3grams" << std::endl;
      return;
    }
    std::cout << "Processing file 3grams" << std::endl;
    int count = 0;
    int n;
    std::string word1, word2, word3;
    while(file >> n >> word1 >> word2 >> word3){
      count++;
      const MWord* mw1 = words[word1];
      const MWord* mw2 = words[word2];
      const MWord* mw3 = words[word3];
      if(mw1 == nullptr || mw2 == nullptr || mw3 == nullptr) // words not considered as 1gram
	continue;

      auto it = gram3Conts.find({mw1,mw2});
      if(it == gram3Conts.end()){
	gram3Conts[{mw1,mw2}] = ContList();
	it = gram3Conts.find({mw1,mw2});
      }
      it->second.total += n;
      it->second.list.push_back({n,mw3});
    }
    file.close();
    std::cout << "Loaded " << count << " 3grams." << std::endl;
  }
  
}

const MWord* Words::GetRandomSentenceBeg(bool proportional){
  if(total_sentence_begs == 0) return nullptr;
  if(proportional){
    int q = rand()%total_sentence_begs;
    for(const auto& pair : sentence_begs){
      q -= pair.first;
      if(q <= 0) return pair.second;
    }
  }else{
    int q = rand()%(sentence_begs.size());
    return sentence_begs[q].second;
  }
}

const MWord* Words::GetRandom1Gram(bool proportional){
  if(total_1grams_amt == 0) return nullptr;
  if(proportional){
    int q = rand()%total_1grams_amt;
    for(const auto& pair : grams1){
      q -= pair.first;
      if(q <= 0) return pair.second;
    }
  }else{
    int q = rand()%(grams1.size());
    return grams1[q].second;
  }
}

const MWord* Words::ContList::GetRandom(bool proportional){
  if(total == 0) return nullptr;
  if(proportional){
    int q = rand()%total;
    for(const auto& pair : list){
      q -= pair.first;
      if(q <= 0) return pair.second;
    }
  }else{
    int q = rand()%(list.size());
    return list[q].second;
  }
}


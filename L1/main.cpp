#include "interpreter.hpp"
#include "grams.hpp"
#include "utf8.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

Words w;
bool proportional = true;
int max_level = 3;

void hello(){
  std::cout << "Hello!" << std::endl;
}

void PrintContinuations(std::string word){
  // std::string word = "teraz";
  const MWord* mw = w.words[word];
  if(mw == nullptr){
    std::cout << "Unknown word: " << word << std::endl;
    return;
  }
  Words::ContList& gc = w.gram2Conts[mw];
  std::cout << "Word: \"" << word << "\", total = " << gc.total << std::endl;
  for(auto it = gc.list.rbegin(); it != gc.list.rend(); it++){
    std::cout << it->first << "\t" << it->second->word << std::endl;
  }
}

void PrintContinuations3(std::string word1, std::string word2){
  const MWord* mw1 = w.words[word1];
  const MWord* mw2 = w.words[word2];
  if(mw1 == nullptr){
    std::cout << "Unknown word: " << word1 << std::endl;
    return;
  }
  if(mw2 == nullptr){
    std::cout << "Unknown word: " << word2 << std::endl;
    return;
  }
  Words::ContList& gc = w.gram3Conts[{mw1,mw2}];
  std::cout << "Words: \"" << word1 << " + " << word2 << "\", Total = " << gc.total << std::endl;
  for(auto it = gc.list.rbegin(); it != gc.list.rend(); it++){
    std::cout << it->first << "\t" << it->second->word << std::endl;
  }  
}

void EchoNum(int x){
  std::cout << x << std::endl;
}

void SetProportional(int p){
  proportional = p;
}

std::string Sentence(){
  std::string result;
  const MWord* mw = w.GetRandomSentenceBeg(proportional);
  result += UTF8::uppercase_first(mw->word) + " ";
  int count = 1;
  const MWord* last2 = nullptr, *last1 = nullptr;
  while(1){
    last2 = last1;
    last1 = mw;
    mw = nullptr;

    if(max_level >= 3 && mw == nullptr && last2 != nullptr){
      auto it = w.gram3Conts.find({last2,last1});
      if(it != w.gram3Conts.end())
	mw = it->second.GetRandom(proportional);
    }
    if(max_level >= 2 && mw == nullptr){
      auto it = w.gram2Conts.find(last1);
      if(it != w.gram2Conts.end())
	mw = it->second.GetRandom(proportional);
    }
    if(mw == nullptr){
      mw = w.GetRandom1Gram(proportional);
    }
    if(mw == nullptr){
      // Still cannot find any word? Abort.
      break;
    }
    result += mw->word + " ";
    if(mw->IsEnding()) break;
    count++;
    if(count > 100){
      std::cout << "--- Sentence too long, breaking." << std::endl;
      break;
    }
  }
  return result;
}

void PrintSentence(){
  std::cout << Sentence() << std::endl;
}

int main(){
  std::srand(std::time(0));
  w.Init();
  Interpreter i;
  i.AddCommand("hello","Greeeting",hello);
  i.AddCommandArgs<std::string,std::string>("find3","Lists possible continuations for a given word, by most frequent 3grams. Requires 2 words as arguments.", PrintContinuations3);
  i.AddCommandArgs<std::string>("find2","Lists possible continuations for a given word, by most frequent 2grams. Requires 1 word as argument.", PrintContinuations);
  i.AddCommandArgs<int>("proportional","Enables/disables proportional mode (0/1)",SetProportional);
  i.AddCommand("sentence","Generates a random sentence.",PrintSentence);
  i.AddCommandArgs<int>("level","Sets the maximum n for ngrams that should be used while generating sentences.",[](int n){max_level = n;});
  i.Run();
}

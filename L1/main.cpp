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

void PrintPermutations(){
  std::string sentence;
  std::cout << "Please enter a sentence:" << std::endl;
  std::getline(std::cin, sentence);
  utf8string s = sentence;
}

void PrintContinuations(std::string word){
  // std::string word = "teraz";
  Words::ContList& gc = w.gram2Conts[word];
  std::cout << "Word: \"" << word << "\", total = " << gc.total << std::endl;
  for(auto it = gc.list.rbegin(); it != gc.list.rend(); it++){
    std::cout << it->first << "\t" << *(it->second) << std::endl;
  }
}

void PrintContinuations3(std::string word1, std::string word2){
  Words::ContList& gc = w.gram3Conts[{word1,word2}];
  std::cout << "Words: \"" << word1 << " + " << word2 << "\", Total = " << gc.total << std::endl;
  for(auto it = gc.list.rbegin(); it != gc.list.rend(); it++){
    std::cout << it->first << "\t" << *(it->second) << std::endl;
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
  MWord word = w.GetRandomSentenceBeg(proportional);
  result += UTF8::uppercase_first(word) + " ";
  int count = 1;
  MWord last2 = "", last1 = "";
  while(1){
    last2 = last1;
    last1 = word;
    word = "";

    if(max_level >= 3 && word == "" && last2 != ""){
      auto it = w.gram3Conts.find({last2,last1});
      if(it != w.gram3Conts.end())
	word = it->second.GetRandom(proportional);
    }
    if(max_level >= 2 && word == ""){
      auto it = w.gram2Conts.find(last1);
      if(it != w.gram2Conts.end())
	word = it->second.GetRandom(proportional);
    }
    if(word == ""){
      word = w.GetRandom1Gram(proportional);
    }
    if(word == ""){
      // Still cannot find any word? Abort.
      break;
    }
    result += word + " ";
    if(word.IsEnding()) break;
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
  i.AddCommand("permutations", "Prints a list of all possible word permutations from a sentence.",PrintPermutations);
  i.Run();
}

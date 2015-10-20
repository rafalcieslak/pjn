#include "interpreter.hpp"
#include "utf8.hpp"
#include "grams2.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <set>
#include <unordered_map>
#include "tuple_hash.hpp"

bool proportional = true;
int max_level = 3;
int limit = 1000000;
unsigned int min_occ1 = 2, min_occ2 = 3, min_occ3 = 2;

void hello(){
  std::cout << "Hello!" << std::endl;
}

void PrintPermutations(){
  std::string sentence;
  std::cout << "Please enter a sentence:" << std::endl;
  std::getline(std::cin, sentence);
  utf8string s = sentence;
  std::vector<utf8string> v = s.words();
  std::sort(v.begin(), v.end());
  do{
    for(auto word : v){
      std::cout << word << " ";
    }
    std::cout << std::endl;
  }while(std::next_permutation(v.begin(), v.end()));
}

void PrintConfig(){
  std::cout << "limit = " << limit << std::endl;
  std::cout << "proportional = " << proportional << std::endl;
  std::cout << "level = " << max_level << std::endl;
  std::cout << "minocc = " << min_occ1 << " " << min_occ2 << " " << min_occ3 << std::endl;
}

void Sentence(){
  utf8string word = Words2::Query1Grams("*",limit,min_occ1).GetRandom(proportional)[0];
  std::cout << UTF8::uppercase_first(word) << " " << std::flush;
  int count = 1;
  utf8string last2 = "", last1 = "";
  while(1){
    last2 = last1;
    last1 = word;
    word = "";

    if(max_level >= 3 && word == "" && last2 != ""){
      auto r = Words2::Query3Grams(last2, last1, "*", limit, min_occ3);
      if(r.total != 0)
	word = r.GetRandom(proportional).back();
    }
    if(max_level >= 2 && word == "" && last1 != ""){
      auto r = Words2::Query2Grams(last1, "*", limit, min_occ2);
      if(r.total != 0)
	word = r.GetRandom(proportional).back();
    }
    if(word == ""){
      auto r = Words2::Query1Grams("*", limit, min_occ1);
      if(r.total != 0)
	word = r.GetRandom(proportional).back();
    }
    if(word == ""){
      // Still cannot find any word? Abort.
      break;
    }
    std::cout << word << " " << std::flush;
    if(word.isSentenceEnding()) break;
    count++;
    if(count > 100){
      std::cout << "--- Sentence too long, breaking.";
      break;
    }
  }
  std::cout << std::endl;
}

typedef std::tuple<utf8string> ts1;
typedef std::tuple<utf8string,utf8string> ts2;
typedef std::tuple<utf8string,utf8string,utf8string> ts3;

static std::unordered_map<ts1,unsigned int> cache1;
static std::unordered_map<ts2,unsigned int> cache2;
static std::unordered_map<ts3,unsigned int> cache3;

template <typename Key, typename Value>
Value GetCached_i(Key key, std::unordered_map<Key,Value>& cache, std::function<Value(Key)> getter){
  auto it = cache.find(key);
  if(it == cache.end()){
    Value val = getter(key);
    cache[key] = val;
    return val;
  }else{
    return it->second;
  }
}

template <typename Key, typename Value, typename F>
Value GetCached(Key key, std::unordered_map<Key,Value>& cache, F&& getter){
  return GetCached_i(key, cache, std::function<Value(Key)>(getter));
}

double Analyse(std::vector<utf8string> sentence, bool verbose = false){
  double score = 0.0;
  if(verbose) std::cout << "Analysing sentence." << std::endl;
  int n = sentence.size();
  if(verbose) std::cout << "Unigrams:" << std::endl;
  unsigned int amt;
  for(int i = 0; i < n; i++){
    amt = GetCached(ts1(sentence[i]), cache1, [&](ts1 key){return Words2::Query1Grams(std::get<0>(key), limit, min_occ1).total;});
    if(verbose) std::cout << amt << " " << std::flush;
  }
  if(verbose) std::cout << std::endl << "Bigrams:" << std::endl;
  for(int i = 0; i < n-1; i++){
    amt = GetCached(ts2(sentence[i],sentence[i+1]), cache2, [&](ts2 key){return Words2::Query2Grams(std::get<0>(key), std::get<1>(key), limit, min_occ2).total;});
    if(verbose) std::cout << amt << " " << std::flush;
    //if(i > 0 && i < n-2)
      score += log((double)amt + 1.0);
  }
  if(verbose) std::cout << std::endl << "Trigrams:" << std::endl;
  for(int i = 0; i < n-2; i++){
    amt = GetCached(ts3(sentence[i],sentence[i+1],sentence[i+2]), cache3, [&](ts3 key){return Words2::Query3Grams(std::get<0>(key), std::get<1>(key), std::get<2>(key), limit, min_occ3).total;});
    if(verbose) std::cout << amt << " " << std::flush;
    score += log((double)amt + 1.0);
  }
  if(verbose) std::cout << std::endl << "Score: 1/N * exp( " << score << " )" << std::endl;
  return score;
}

void Auto(std::vector<utf8string> v /*sentence*/){
  std::sort(v.begin(), v.end());
  int perms = tgamma(v.size() + 1);
  int n = 1;
  std::set<std::pair<double, utf8string>> results;
  utf8string last;
  double score = 0.0;
  do{
    std::cout  << "Analyzing sentence perutation " << n << "/" << perms << "... " << std::flush;
    score = Analyse(v);
    last = JoinString(v);
    results.insert({score,last});
    std::cout << "Result: sentence = \"" << last << "\", score = " << score << std::endl;
    n++;
  }while(std::next_permutation(v.begin(), v.end()));
  std::cout << std::endl <<  " ==== RESULTS ==== " << std::endl;
  for(auto it = results.rbegin(); it != results.rend(); it++){\
    double score = it->first;
    auto sentence = it->second;
    std::cout << score << "\t" << sentence << std::endl;
  }
}

int main(){
  std::srand(std::time(0));

  PrintConfig();
  
  Interpreter i;
  i.AddCommandArgs<int>("proportional","Enables/disables proportional mode (0/1)",[](int n){proportional = n;});
  i.AddCommand("sentence","Generates a random sentence.",Sentence);
  i.AddCommandArgs<int>("level","Sets the maximum n for ngrams that should be used while generating sentences.",[](int n){max_level = n;});
  i.AddCommand("permutations", "Prints a list of all possible word permutations from a sentence.",PrintPermutations);
  i.AddCommandArgs<int>("limit", "Sets the maximum number of results from a query.", [](int n){limit = n;});
  i.AddCommandArgs<int,int,int>("minocc", "Sets the minumum number of occurences a gram needs to be even considered.", [](int a, int b, int c){min_occ1 = a; min_occ2 = b; min_occ3 = c;});
  i.AddCommand("config","Prints current configuration.", PrintConfig);
  i.AddCommandArgs<std::string>("query1","Searches through 1grams",[](std::string s){
      Words2::Query1Grams(utf8string(s),limit,min_occ1).Print();
  });
  i.AddCommandArgs<std::string,std::string>("query2","Searches through 2grams",[](std::string s1, std::string s2){
      Words2::Query2Grams(utf8string(s1),utf8string(s2),limit,min_occ2).Print();
  }); 
  i.AddCommandArgs<std::string,std::string,std::string>("query3","Searches through 3grams",[](std::string s1, std::string s2, std::string s3){
      Words2::Query3Grams(utf8string(s1),utf8string(s2),utf8string(s3),limit,min_occ3).Print();
  });   
  i.AddCommand("analyse", "Analyzes grams taken from input sentence", [](){
      std::string str;
      std::cout << "Please enter a sentence to analyze." << std::endl;
      std::getline(std::cin,str);
      auto s = utf8string(str).words();
      Analyse(s,true);
  });
  i.AddCommand("auto", "Automatically finds most/least probable sentence permutations", [](){
      std::string str;
      std::cout << "Please enter a sentence to analyze." << std::endl;
      std::getline(std::cin,str);
      auto s = utf8string(str).words();
      Auto(s);
    });
  
  i.Run();
  
}

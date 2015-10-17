#include "grams2.hpp"
#include <fstream>
#include <iostream>

char buffer[1000000];

Words2::QueryResults<1> Words2::Query1Grams(Words2::Pattern p1, unsigned int limit, unsigned int min_occ){
  QueryResults<1> res;
  std::string filename = "1grams";
  std::ifstream file;
  file.rdbuf()->pubsetbuf(buffer, sizeof(buffer));
  file.open(filename);
  if(!file){
    std::cout << "Failed to open file: " << filename << std::endl;
    return res;
  }
  //  std::cout << "Query in file " << filename << "..." << std::endl;
  unsigned int n, count = 0;
  if(p1.pt == Pattern::FULL) limit = 1;
  std::array<utf8string,1> a;
  while(file >> n >> a[0]){
    if(n < min_occ) break;    
    if( !p1.Matches(a[0]) ) continue;
    res.list.push_back({n,a});
    res.total += n;
    if(count++ >= limit) break;
  }
  file.close();

  return res;
}

Words2::QueryResults<2> Words2::Query2Grams(Words2::Pattern p1, Words2::Pattern p2, unsigned int limit, unsigned int min_occ){
  QueryResults<2> res;
  std::string filename = "2grams";
  std::ifstream file;
  file.rdbuf()->pubsetbuf(buffer, sizeof(buffer));
  file.open(filename);
  if(!file){
    std::cout << "Failed to open file: " << filename << std::endl;
    return res;
  }
  // std::cout << "Query in file " << filename << "..." << std::endl;
  unsigned int n, count = 0;
  if(p1.pt == Pattern::FULL && p2.pt == Pattern::FULL) limit = 1;
  std::array<utf8string,2> a;
  while(file >> n >> a[0] >> a[1]){
    if(n < min_occ) break;
    if( !p1.Matches(a[0]) || !p2.Matches(a[1]) ) continue;
    res.list.push_back({n,a});
    res.total += n;
    if(count++ >= limit) break;
  }
  file.close();

  return res;
}


Words2::QueryResults<3> Words2::Query3Grams(Words2::Pattern p1, Words2::Pattern p2, Words2::Pattern p3, unsigned int limit, unsigned int min_occ){
  QueryResults<3> res;
  std::string filename = "3grams";
  std::ifstream file;
  file.rdbuf()->pubsetbuf(buffer, sizeof(buffer));
  file.open(filename);
  if(!file){
    std::cout << "Failed to open file: " << filename << std::endl;
    return res;
  }
  // std::cout << "Query in file " << filename << "..." << std::endl;
  unsigned int n, count = 0;
  if(p1.pt == Pattern::FULL && p2.pt == Pattern::FULL && p3.pt == Pattern::FULL) limit = 1;
  std::array<utf8string,3> a;
  while(file >> n >> a[0] >> a[1] >> a[2]){
    if(n < min_occ) break;
    if( !p1.Matches(a[0]) || !p2.Matches(a[1]) || !p3.Matches(a[2]) ) continue;
    res.list.push_back({n,a});
    res.total += n;
    if(count++ >= limit) break;
  }
  file.close();

  return res;
}

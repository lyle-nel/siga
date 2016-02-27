#ifndef LIB_H
#define LIB_H

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>

namespace lib
{
  template <class Container>
  static void fill_from_file(std::string filename, Container& in_out)
  {
    std::ifstream ifile(filename.c_str());
    if(!ifile.is_open())
      std::cerr << "Cannot open file: " << filename << std::endl;
    std::string temp;
    while (std::getline(ifile, temp))
      in_out.insert(in_out.end(),std::string(temp));
  }

  static std::string generate_random_string(const std::string& source, size_t length)
  {
    Random_dist rand;
    auto random_char = [&source,&rand](){return source[rand.uniform_int(0,source.size()-1)];};

    std::string tmp(length,0);
    std::generate_n(tmp.begin(),length,random_char);
    return tmp;
  }
}
#endif

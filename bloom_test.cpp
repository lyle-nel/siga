#include "library/bloom_filter.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

int main()
{
  std::vector<std::string> test_data = {"super", "kala", "fragi", "listic", "expi", "ali", "docious"};
  std::cout << Bloom_filter::minimum_false_positives(test_data.size(),1024) << std::endl;
  Bloom_filter bf = Bloom_filter(test_data.size(), Bloom_filter::minimum_false_positives(test_data.size(),1024));
  for(auto str:test_data)
    bf.add(str);
  test_data.push_back("non_existing_string");
//  std::cout << bf.hash_count << std::endl;
  for(auto str:test_data)
    std::cout << bf.maybe_exists(str) << std::endl;
  std::cout << std::fixed;
   std::cout << std::setprecision(100) << Bloom_filter::minimum_false_positives(4000000000,(size_t)1024*1024*1024*8) << std::endl;
}

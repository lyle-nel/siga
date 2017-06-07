#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include <vector>
#include <string>
#include <stdlib.h>
#include <cmath>

#include <iostream>

class Bloom_filter
{
public:
  static long double minimum_false_positives(size_t element_count, size_t size_limit)
  {
//    return (   exp(  -( size_limit*pow(log(2),2) )/element_count  )   );
    return pow((1-exp(-((size_limit/element_count)*log(2))*element_count/size_limit)),(size_limit/element_count)*log(2));
  }
  Bloom_filter(){}
  Bloom_filter(size_t elements_count, long double false_positive_rate)
  {
//    size_t bitsize = ceil(-((double)elements_count*pow(log(2),2))/(log(false_positive_rate)));
    size_t bitsize = ceil(    -(elements_count*log(false_positive_rate)) / (pow(log(2.0),2.0))    );
    hash_count = round(log(2.0) * (bitsize / elements_count));
    std::cout << "bitsize: " << bitsize << std::endl;
    bitvector = std::vector<bool>(bitsize,false);
    hash2_nonce = "bzAvYdcZlSzxwJYQIlLGrbpd2";
  }

  void add(const std::string& str)
  {
    size_t hash1 = std::hash<std::string>()(str);
    size_t hash2 = std::hash<std::string>()(str+hash2_nonce);
    for(size_t n = 0; n!=hash_count; ++n)
      bitvector[nth_hash(hash1, hash2, n, bitvector.size())] = true;
  }

  bool maybe_exists(const std::string& str)
  {
    size_t hash1 = std::hash<std::string>()(str);
    size_t hash2 = std::hash<std::string>()(str+hash2_nonce);
    for(size_t n = 0; n!=hash_count; ++n)
      if(!bitvector[nth_hash(hash1, hash2, n, bitvector.size())])
        return false;
    return true;
  }
private:
  size_t nth_hash(size_t hash1, size_t hash2, size_t n, size_t size)
  {
    return (hash1 + n*hash2)%size;
  }
  std::vector<bool> bitvector;
  size_t hash_count;
  std::string hash2_nonce;
};
#endif

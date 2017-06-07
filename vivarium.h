#ifndef VIVARIUM_H
#define VIVARIUM_H

#include "library/random_dist.h"
#include "library/bloom_filter.h"
#include <deque>
#include <vector>
#include <unordered_set>

struct Organism
{
  std::string gene;
};

class Vivarium {
public:
  Vivarium(Random_dist& rand);
  bool matches(const std::string& in);
  std::string calc_hash(const std::string& in) const;
  void mutate(Organism& organism) const;
  int crossover(const Organism& lhs, Organism& rhs) const;
  void run();

private:

  std::vector<std::string> source_text;

//  std::unordered_set<std::string> hashes;
  Bloom_filter uncracked;
  Bloom_filter cracked;

  std::deque<Organism> pool;
  Random_dist& random_engine;
};
#endif

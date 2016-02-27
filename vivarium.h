#ifndef VIVARIUM_H
#define VIVARIUM_H

#include "library/random_dist.h"
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
  std::string calc_hash(const std::string& in);
  void mutate(Organism& organism);
  int crossover(const Organism& lhs, Organism& rhs);
  void run();
  void deredundant();
  double shannon_word_enthropy(const std::string in);

private:

  std::vector<std::string> source_text;

  std::unordered_set<std::string> hashes;

  std::deque<Organism> pool;
  Random_dist& random_engine;
};
#endif

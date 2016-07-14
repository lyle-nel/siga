#ifndef SETTINGS_H
#define SETTINGS_H
#include <string>
#include <limits>

namespace setting
{
  //Simulation settings
  extern bool dump_candidates;
  extern double mutation_rate;
  extern double multi_parent_crossover_prob;
  extern size_t init_population;
  extern size_t max_population;
  extern bool md5_mode;
  extern bool sha1_mode;
  extern bool interactive;
  extern bool generate_random_organisms;
  extern bool verbose;
  //File settings
  extern std::string organism_file;
  extern std::string training_file;
  extern std::string cracked_file;

  extern std::string char_list;
}

#endif

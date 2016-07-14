#include "setting.h"
namespace setting
{
  //Simulation settings
  bool dump_candidates = false;
  double mutation_rate = 0.01;
  double multi_parent_crossover_prob = 0.1;
  size_t init_population = 100;
  size_t max_population = 50000;
  bool md5_mode = false;
  bool sha1_mode = false;
  bool interactive = false;
  bool generate_random_organisms = false;
  bool verbose = false;
 //File settings
  std::string organism_file = "data/organism.txt";
  std::string training_file= "data/training.txt";
  std::string cracked_file = "data/cracked.txt";
  std::string char_list = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$^&*()_+-=|\\}]{[:;\"\'<,>.?/";
}

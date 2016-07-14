#include <iostream>
#include <boost/program_options.hpp>
#include "vivarium.h"
#include "library/random_dist.h"
#include "setting.h"

using namespace boost::program_options;

int main(int argc, const char *argv[])
{
  try
  {
    options_description desc{"Options"};
    desc.add_options()
      ("help,h", "Help screen")
      ("verbose,v", "Print some information on the simulation, such as startup information and what words have been added in interactive mode ,provided interactive mode is enabled.")
      ("max_population", value<size_t>()->default_value(0), "The maximum size of the pool of organisms. If this value is too small, the population cannot express sufficient diversity and it gets stuck. Overall there is not much of a disadvantage to an uncapped (=0) poplulation growth.")
      ("init_population", value<size_t>()->default_value(setting::init_population), "The initial population size of random organisms when no filename organism_file hash been specified.")
      ("mutation_rate", value<double>()->default_value(setting::mutation_rate), "Mutation rate of organisms. If the mutation rate is too high, organisms will be unable to fully exploit novel solution.")
      ("multi_parent_crossover_prob", value<double>()->default_value(setting::multi_parent_crossover_prob), "The probability that an organism will have more than 2 parents. This can be advantageous when there exist words(offspring) that can only be composed of more than 2 other words(parents).")
      ("char_list", value<std::string>()->default_value(setting::char_list), "List of characters available to be used to generate random organisms as well as mutations of existing organisms. The character space can always be expanded later with interactive mode.")
      ("md5_mode", "Run the simulation in solo mode. In this mode the training_file is a list md5 hashes to be cracked.")
      ("sha1_mode", "Same as md5_mode, but just for sha1")
      ("dump_candidates", "Dumps all candidate passwords that are trained from training_file.")
      ("interactive", "Takes hints from stdin. Swearwords and linear keyboard sequences such as qwertyuiop seem to work well with most password lists. This can also be used to expand the character space by entering new characters that are not part of the original char_list.")
      ("organism_file", value<std::string>(), "List of initial organisms to seed the simulation. If this argument is not supplied, the simulation will start with random organisms with a population size of init_population.")
      ("training_file", value<std::string>()->default_value(setting::training_file), "The list of words to train organisms to. In md5 mode, this is the list of passwords to crack")
      ("cracked_file", value<std::string>()->default_value(setting::cracked_file), "Words successfully found are dumped here.");
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if(vm.count("help"))
    {
      std::cout << desc << '\n';
      return 0;
    }

    if(vm.count("verbose"))
      setting::verbose = true;
    else
      setting::verbose = false;

    if(vm.count("max_population"))
      setting::max_population = vm["max_population"].as<size_t>();

    if(vm.count("init_population"))
      setting::init_population = vm["init_population"].as<size_t>();

    if(vm.count("mutation_rate"))
      setting::mutation_rate = vm["mutation_rate"].as<double>();

    if(vm.count("multi_parent_crossover_prob"))
      setting::multi_parent_crossover_prob = vm["multi_parent_crossover_prob"].as<double>();

    if(vm.count("char_list"))
      setting::char_list = vm["char_list"].as<std::string>();

    if(vm.count("md5_mode"))
      setting::md5_mode = true;
    else
      setting::md5_mode = false;
    if(vm.count("sha1_mode"))
      setting::sha1_mode = true;
    else
      setting::sha1_mode = false;

    if(vm.count("dump_candidates"))
      setting::dump_candidates = true;
    else
      setting::dump_candidates = false;

    if(vm.count("interactive"))
      setting::interactive = true;
    else
      setting::interactive = false;

    if(vm.count("organism_file"))
    {
      setting::generate_random_organisms = false;
      setting::organism_file = vm["organism_file"].as<std::string>();
    }
    else
      setting::generate_random_organisms = true;

    if(vm.count("training_file"))
      setting::training_file = vm["training_file"].as<std::string>();

    if(vm.count("cracked_file"))
      setting::cracked_file = vm["cracked_file"].as<std::string>();
  }
  catch (const error &ex)
  {
    std::cerr << ex.what() << '\n';
  }
  Random_dist rand;
  Vivarium vivarium(rand);
  vivarium.run();
}

#include "vivarium.h"
#include "setting.h"

#include <iostream>
#include <algorithm>
#include <array>
#include <fstream>
#include <functional>//std::function
#include "library/lib.h"

Vivarium::Vivarium(Random_dist& rand):random_engine(rand)
{
  if(setting::verbose)
    std::cerr << "Loading files" << std::endl;

  if(!setting::generate_random_organisms)
    lib::fill_from_file(setting::organism_file.c_str(), source_text);

  lib::fill_from_file(setting::training_file.c_str(), hashes);
  if(setting::verbose)
  {
    std::cerr << "Done" << std::endl;
    std::cerr << "Computing hashes" << std::endl;
    std::cerr << "Initial hash list size" << hashes.size() << std::endl;
  }
  if(!setting::generate_random_organisms)
    for(auto& word: source_text)
    {
      auto it = hashes.find(calc_hash(word));
      if(it != hashes.end())
        hashes.erase(it);
    }
  if(setting::verbose)
  {
    std::cerr << "Final hash list size" << hashes.size() << std::endl;
    std::cerr << "Done" << std::endl;
    std::cerr << "Creating initial organisms" << std::endl;
  }
  if(setting::generate_random_organisms)
  {
    for(size_t i = 0; i!=setting::init_population; ++i)
    {
      size_t random_length = random_engine.uniform_int(1,3);
      Organism random_organism = {lib::generate_random_string(setting::char_list, random_length)};
      pool.push_back(random_organism);
    }
  }
  else
  {
    for(auto& word: source_text)
    {
      Organism organism = {word};
      pool.push_back(organism);
    }
  }
  if(setting::verbose)
    std::cerr << "Done" << std::endl;
}

#include "library/md5.h"
#include "library/sha1.h"
std::string Vivarium::calc_hash(const std::string& in) const
{
  if(setting::md5_mode)
    return md5(in.c_str());
  if(setting::sha1_mode)
    return sha1(in);
  return in;//passthrough. No need for performance penalty
}

bool Vivarium::matches(const std::string& in)
{
  std::string hash = calc_hash(in);
  auto it = hashes.find(hash);
  if(it == hashes.end())
    return false;
  hashes.erase(it);
  return true;
}

#include <mutex>
#include <thread>
void Vivarium::run()
{
  std::ofstream cracked(setting::cracked_file.c_str());

  std::mutex pool_mutex;
  std::thread user_input;

  if(setting::interactive)
    user_input = std::thread
    (
      [this,&pool_mutex]()
      {
        while(true)
        {
          if(setting::verbose)
            std::cerr << "Give me a hint:";
          Organism organism;
          std::getline(std::cin, organism.gene);
          std::lock_guard<std::mutex> lock(pool_mutex);
          pool.push_back(organism);
          if(setting::verbose)
            std::cerr << organism.gene << " was added" << std::endl;
        }
      }
    );

  while(true)
  {
    size_t parent_1_index = random_engine.uniform_int(0,pool.size()-1);
    const size_t parent_2_index = random_engine.reverse_exponential_int(pool.size()-1);
    auto parent_2 = pool[parent_2_index];

    mutate(parent_2);
    do
    {
      crossover(pool[parent_1_index], parent_2);
      parent_1_index = random_engine.uniform_int(0,pool.size()-1);
    }while(random_engine.bernoulli(setting::multi_parent_crossover_prob));//chance to combine dna of more than 2 organisms

    if(setting::dump_candidates)
      std::cout << parent_2.gene << '\n';

    if(matches(parent_2.gene))
    {
      cracked << parent_2.gene << std::endl;
      std::lock_guard<std::mutex> lock(pool_mutex);
      pool.push_back(parent_2);
    }

    //remove an organism if the max_vivarium_size has been exeeded, provided setting::max_vivarium_size is not 0, which means there is no maximum size(it is infinite)
    if(pool.size()>setting::max_population && setting::max_population!=0)
    {
      std::lock_guard<std::mutex> lock(pool_mutex);
      pool.pop_front();
    }
  }
}

//std::cout << "skew_1: " << i_rand/(double)pool.size() << std::endl;
//std::cout << "skew_2: " << further_along/(double)pool.size() << std::endl;


void Vivarium::mutate(Organism& organism) const
{
  if(!random_engine.bernoulli(setting::mutation_rate))
    return;//high chance of not mutating

  auto substitute_mutate = [this](std::string& gene)
  {
    if(gene.size() == 0) return;
    size_t random_length = random_engine.uniform_int(1,5);
    std::string random_string = lib::generate_random_string(setting::char_list, random_length);
    size_t position = random_engine.uniform_int(0,gene.size()-1);
    gene.replace(position, random_string.size(), random_string);
  };
  auto insert_mutate = [this](std::string& gene)
  {
    size_t random_length = random_engine.uniform_int(1,5);
    std::string random_string = lib::generate_random_string(setting::char_list, random_length);

    size_t position = 0;
    if(gene.size() != 0)
      position = random_engine.uniform_int(0,gene.size()-1);
    gene.insert(position, random_string);
  };
  auto delete_mutate = [this](std::string& gene)
  {
    if(gene.size() == 0) return;
    size_t position = random_engine.uniform_int(0,gene.size()-1);
    size_t random_length = random_engine.uniform_int(1,gene.size()-position-1);
    gene.erase(position, random_length);
  };

  auto reverse_mutate = [this](std::string& gene)
  {
    if(gene.size() == 0) return;
    std::reverse(gene.begin(), gene.end());
  };

  std::array<std::function<void(std::string&)>, 4> strategy =
  {
    substitute_mutate,
    insert_mutate,
    delete_mutate,
    reverse_mutate
  };

  auto mutation_strategy = strategy[random_engine.uniform_int(0,strategy.size()-1)];
  mutation_strategy(organism.gene);
}

int Vivarium::crossover(const Organism& lhs, Organism& rhs) const
{
  auto full_substitute = [this](const Organism& lhs, Organism& rhs)
  {
    if(lhs.gene.size() == 0) return;

    size_t position = 0;
    if(rhs.gene.size() != 0)
      position = random_engine.uniform_int(0,rhs.gene.size()-1);
    rhs.gene.replace(position, lhs.gene.size(), lhs.gene);
  };
  auto partial_substitute = [this](const Organism& lhs, Organism& rhs)
  {
    if(lhs.gene.size() == 0) return;

    size_t lhs_position = random_engine.uniform_int(0,lhs.gene.size()-1);
    size_t substr_length = random_engine.uniform_int(1,lhs.gene.size()-1);

    size_t position = 0;
    if(rhs.gene.size() != 0)
      position = random_engine.uniform_int(0,rhs.gene.size()-1);
    rhs.gene.replace(position, substr_length, lhs.gene.substr(lhs_position,substr_length));
  };
  auto full_insert = [this](const Organism& lhs, Organism& rhs)
  {
    if(lhs.gene.size() == 0) return;

    size_t position = 0;
    if(rhs.gene.size() != 0)
      position = random_engine.uniform_int(0,rhs.gene.size()-1);
    rhs.gene.insert(position, lhs.gene);
  };
  auto partial_insert = [this](const Organism& lhs, Organism& rhs)
  {
    if(lhs.gene.size() == 0) return;

    size_t lhs_position = random_engine.uniform_int(0,lhs.gene.size()-1);
    size_t substr_length = random_engine.uniform_int(1,lhs.gene.size()-1);

    size_t rhs_position = 0;
    if(rhs.gene.size() != 0)
      rhs_position = random_engine.uniform_int(0,rhs.gene.size()-1);
    rhs.gene.insert(rhs_position, lhs.gene, lhs_position, substr_length);
  };

  //by emperical experimentation, full_substitute and full_insert are rarely used in successfully cracked words
  std::array<std::function<void(const Organism&,Organism&)>, 2> strategy =
  {
//    full_substitute,
    partial_substitute,
//    full_insert,
    partial_insert
  };
  size_t selected_strategy = random_engine.uniform_int(0,strategy.size()-1);
  auto crossover_strategy = strategy[selected_strategy];
  crossover_strategy(lhs,rhs);
}

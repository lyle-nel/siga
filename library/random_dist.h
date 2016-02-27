#ifndef RANDOM_DIST_H
#define RANDOM_DIST_H
#include <random>
#include <chrono>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/lagged_fibonacci.hpp>
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/uniform_int_distribution.hpp>
class Random_dist//TODO: save seed value to file to replay a specific experimental instance
{
public:
  Random_dist()
  :engine(std::chrono::system_clock::now().time_since_epoch().count()), float_engine(std::chrono::system_clock::now().time_since_epoch().count()){}

  int uniform_int(int min, int max)
  {
    return std::uniform_int_distribution<int>{min, max}(engine);
  }

  int reverse_exponential_int(int max)
  {
    auto exp_dist = std::exponential_distribution<>(5);
    double expon = exp_dist(engine)*(double)max;
    int expon_casted = static_cast<int>(expon);
    expon_casted = max-expon_casted;//reverse the distribution
    while(expon_casted<0)
    {
      exp_dist = std::exponential_distribution<>(5);
      expon = exp_dist(engine)*(double)max;
      expon_casted = static_cast<int>(expon);
      expon_casted = max-expon_casted;//reverse the distribution
    }
    return expon_casted;
  }

  int exponential_int(int max)
  {
    auto exp_dist = std::exponential_distribution<>(3.5);
    double expon = exp_dist(engine)*(double)max;
    int expon_casted = static_cast<int>(expon);
    while(expon_casted > max)
    {
      exp_dist = std::exponential_distribution<>(3.5);
      expon = exp_dist(engine)*(double)max;
      expon_casted = static_cast<int>(expon);
    }
    expon_casted = std::max(0,std::min(max,expon_casted));//clamp at max
    return expon_casted;
  }

  double uniform_real(double min, double max)
  {
    boost::random::uniform_real_distribution<double> real_dist =
    boost::random::uniform_real_distribution<double>(min,max);
    return real_dist(float_engine);
  }

  bool bernoulli(double p)
  {
    boost::bernoulli_distribution<> dist(p);
    return dist(engine);
  }

  void small_change(double& in_out, const double resolution, const double lower_clamp = 0, const double upper_clamp = 1)
  {
    in_out += uniform_real(-resolution, resolution);
    in_out = std::max(lower_clamp, std::min(in_out, upper_clamp));//clamp in_out between [lower_clamp, upper_clamp]
  }

  void small_change_int(size_t& in_out, const int resolution, const int lower_clamp = 0, const int upper_clamp = 1)
  {
    int change = uniform_int(-resolution, resolution);
    if(in_out == 0 && change < 0)
      return;
    if(change < 0)
      if(in_out < std::numeric_limits<int>::max())//if in_out does not exceed int max then it can become negative by subtraction
        if((change*-1) > in_out)//will it become negative by subtraction?
          return;//yes it will. It will thus underflow and wrap around since it cannot be negative, so do nothing.
    in_out += change;
  }
boost::random::mt11213b engine;
private:
//  std::default_random_engine engine;

boost::random::lagged_fibonacci607 float_engine;//we have a separate engine for bernoulli trials since a rng that produces floating point numbers is better fit for the application.
size_t counter = 0;
};
#endif

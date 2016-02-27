# siga(simple genetic algorithm)
This is a small experiment that focuses on leveraging population-based meta-heuristics for cracking passwords.
We start of with a small vector of random strings or strings from file. With each iteration, we mutate and crossover organisms from random positions until one of their children matches a password. We then push the matched child into the end of the container and pop the oldest organism from the front. This way we have all the properties of a genetic algorithm, with the exception of a fitness function, which is not needed since each organism has a limited lifetime due to them being popped from the front of the container. The above mentioned algorithm is quite effective at preserving high impact substrings that can explain a large number of passwords. As these high impact substring are exhausted, mutations of them or new novel substrings will emerge and start to dominate the gene-pool. Since only a single organism can find a specific solution before that solution is removed from the solution space, the solution space will shrink until only highly complex words remain in the solution space.

Some empirical experimentation shows that the distribution of candidate parents that produce viable offspring are non uniformly distributed in the gene-pool, even though the parents have been picked at random in a uniform fashion. Below are the graphs of the experiment.

![parent 1](https://github.com/lyle-nel/siga/blob/master/documentation/parent_1.png)

![parent 2](https://github.com/lyle-nel/siga/blob/master/documentation/parent_2.png)

Thus, to match our observations, the current algorithm finds the first parent in a uniform manner and the second parent by reversing an exponential distribution in order to approximate the observation. Choosing the second parent non-uniformly with a bias toward the end of the container provides the two additional advantage. Firstly, newer organisms will be given the chance to produce offspring more often than older organisms, which provides sufficient selection pressure so that old organisms do not strictly need to be deleted from the gene-pool since they will lie dormant most of the time. Secondly, since we are not deleting older organisms, the overall diversity of the gene-pool is preserved while providing the chance for dormant organisms to reintroduce their genes in a novel way some time in the future.

For crossover there are 4 strategies, namely, partial_insert, full_insert, partial_substitute and full_substitute. For partial_insert, we take a random substring from the lhs string and inserts it into a random position in rhs string. The full_insert strategy inserts the full lhs string into a random position in rhs string.
In a similar fashion, partial_substitute substitutes a random substring from lhs into a random position in rhs. Finally, full_substitute substitutes the full lhs string into a random position in rhs. By empirical experimentation, I found that mostly partial_insert and partial_substitute is used by successful organisms, so the other 2 are disabled for now. We don't lose any functionality since the 2 activated strategies can fulfill the role of the disabled ones. The disabled ones are, in a sense, a subset of the activated ones.

Mutation of an organism is done in a similar fashion. Mutation strategies include, random insertion, random substitution and random deletion. The lhs string is a random string of length [1,5] and the rhs string is an existing organism we are mutating.

Since we have more than one crossover strategy and mutations strategy, all of them are chosen at random, on the fly, with a uniform distribution.

Future work:
Instead of having the distribution for parent 1 and 2 fixed, it might be useful to have a discrete distribution for both parents and have this discrete distribution updated according to where good candidate parents occur. This way, we try to approximate a distribution that wastes as little time considering bad candidate parents as possible. We know already that these distributions exist by empirical measures. 

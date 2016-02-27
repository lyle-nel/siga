# siga(simple genetic algorithm)

##Dependencies
* It is written in modern c++ and it uses c++11 and c++14 features, so you need a compiler that supports it.
* You need to have Boost installed since program options and the random generator requires it.

##How to build
Assuming you have satified the above dependencies, you can just run the silly makefile. You can then call the binary ./siga -h to see the program options.

##How to use
There are 3 important files:
* data/training.txt: location of the words you want to crack reside.
* data/organism.txt: location of the starting organisms, assuming you dont want to start of with random organisms.
* data/cracked.txt: where all the cracked passwords will be.

The location of these files can be changed by providing the right arguments to siga. Look at ./siga -h for more information.

####Example 1: Cracking md5 passwords starting with a random population of organisms.
Place the hashes in data/training.txt, then call the program with the following options:
```
$ ./siga --md5_mode
```
You can look at the current progress by opening another terminal and running
```
$ tail -f data/cracked.txt
```

####Example 2: Cracking md5 passwords starting with a random population of organisms and I want to set the initial population size to 200.
```
$ ./siga --md5_mode --init_population 200
```

####Example 3: Cracking md5 passwords starting with a random population of organisms and I want to give the program hints as it progresses.
```
$ ./siga --md5_mode --interactive --verbose
```
The verbose option provides you with startup information as well as a prompt to enter the words.

####Example 4: Cracking md5 passwords starting with a random population of organisms and I want to give the program hints as it progresses, but I want the hints to come from a file.
```
$ cat somehints| ./siga --md5_mode --interactive
```
Alternatively if you want to disable the random starting population and use words from the organism file exclusively, you can put your hints into the data/organisms.txt file then run
```
$ ./siga --md5_mode --organism_file=data/organisms.txt
```

####Example 5: I want to bypass the overhead of md5 and do experiments with plaintext datasets.
Just remove the --md5_mode argument.
```
$ ./siga --interactive --verbose
```
The cracked passwords will appear in data/cracked.txt.

##How it works
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

### Future work:
Instead of having the distribution for parent 1 and 2 fixed, it might be useful to have a discrete distribution for both parents and have this discrete distribution updated according to where good candidate parents occur. This way, we try to approximate a distribution that wastes as little time considering bad candidate parents as possible. We know already that these distributions exist by empirical measures. 

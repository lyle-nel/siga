# siga(simple genetic algorithm)
This is a small experiment that focuses on leveraging population-based meta-heuristics for cracking passwords.
Scroll to the end for a description of how it works. When cracking the rock_you password list for 5 minutes, it finds the following top 10 longest passwords
```
16 aaaaaaaaaaaaaaaa
15 laaaaaaaaaaaaaa
15 aaaaaaaaaaaaaaa
14 tristadanielle
14 rachelwilliams
14 princessalinda
14 ilovechris1234
14 aaaaaaaaaaaaaa
13 victormariana
13 sarahmitchell
```

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

####Show me a testrun with the md5 hashed myspace leaked list
Run
```
$ ./siga --md5_mode --organism_file=data/organism.txt
```
Then look at cracked.txt with
```
$ tail -f data/cracked.txt
```
to see which md5 hashes it has managed to crack from the myspace list.

###Other Examples:

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
Just remove the --md5_mode argument. Now data/training.txt can be plaintext passwords.
```
$ ./siga --interactive --verbose
```
The cracked passwords will appear in data/cracked.txt.

####Example 6: I want this program to generate candidate passwords that I can use in hashcat.
Using named pipes we can create a feedback loop that pushes cracked passwords back into siga, so that siga can keep learning.
```
cd hashcat
mkfifo hashcat-2.00/pipe_in
mkfifo hashcat-2.00/pipe_out
```
Then in one terminal run
```
./hashcat-cli64.bin -m 0 -a 0 --segment-size=1 --outfile-format=2 --outfile=pipe_out hashes.txt pipe_in
```
In another terminal run
```
cat hashcat-2.00/pipe_out | ./siga --dump_candidates > hashcat-2.00/pipe_in
```
Now if we want to give siga hints we can just do this
```
echo some_hint > /proc/`pidof siga`/fd/0
```

####Example 7: I want to crack md5 password but I want to stop and resume the simulation at a later time.
Run the program as usual, no additional arguments are required.
```
$ ./siga --md5_mode
#siga gets killed here
```
In order to resume the simulation, all we need to do is copy data/cracked.txt into data/organism.txt
```
$ cp data/cracked.txt data/organism.txt
```
Now we can resume the simulation with the following
```
$ ./siga --md5_mode --organism_file=data/organisms.txt
```
This works because the organisms are plain strings and the cracked.txt file stores them in the exact order they appear in the program during execution. 

##How it works
We start of with a small vector of random strings or strings from file. With each iteration, we mutate and crossover organisms from random positions until one of their children matches a password. We then push the matched child into the end of the container and pop the oldest organism from the front. Here is a simple image that communicates the essentials of the algorithm:

![GA](https://github.com/lyle-nel/siga/blob/master/documentation/GA.png)

This way we have all the properties of a genetic algorithm, with the exception of a fitness function, which is not needed since each organism has a limited lifetime due to them being popped from the front of the container. The above mentioned algorithm is quite effective at preserving high impact substrings that can explain a large number of passwords. As these high impact substring are exhausted, mutations of them or new novel substrings will emerge and start to dominate the gene-pool. Since only a single organism can find a specific solution before that solution is removed from the solution space, the solution space will shrink until only highly complex words remain in the solution space.

Some empirical experimentation shows that the distribution of candidate parents that produce viable offspring are non uniformly distributed in the gene-pool, even though the parents have been picked at random in a uniform fashion. Below are the graphs of the experiment.

![parent 1](https://github.com/lyle-nel/siga/blob/master/documentation/parent_1.png)

![parent 2](https://github.com/lyle-nel/siga/blob/master/documentation/parent_2.png)

Thus, to match our observations, the current algorithm finds the first parent in a uniform manner and the second parent by reversing an exponential distribution in order to approximate the observation. Choosing the second parent non-uniformly with a bias toward the end of the container provides two additional advantages. Firstly, newer organisms will be given the chance to produce offspring more often than older organisms, which provides sufficient selection pressure so that old organisms do not strictly need to be deleted from the gene-pool since they will lie dormant most of the time. Secondly, since we are not deleting older organisms, the overall diversity of the gene-pool is preserved while providing the chance for dormant organisms to reintroduce their genes in a novel way some time in the future.

For crossover there are 4 strategies:
* partial_insert
* full_insert
* partial_substitute
* full_substitute.

For partial_insert, we take a random substring from the lhs string and inserts it into a random position in rhs string. The full_insert strategy inserts the full lhs string into a random position in rhs string.
In a similar fashion, partial_substitute substitutes a random substring from lhs into a random position in rhs. Finally, full_substitute substitutes the full lhs string into a random position in rhs. By empirical experimentation, I found that mostly partial_insert and partial_substitute is used by successful organisms, so the other 2 are disabled for now. We don't lose any functionality since the 2 activated strategies can fulfill the role of the disabled ones. The disabled ones are, in a sense, a subset of the activated ones.

Mutation of an organism is done in a similar fashion. Mutation strategies include
* random insertion
* random substitution
* random deletion

The lhs string is a random string of length [1,5] and the rhs string is an existing organism we are mutating.

Since we have more than one crossover strategy and mutations strategy, all of them are chosen at random, on the fly, with a uniform distribution.

### Future work:
Instead of having the distribution for parent 1 and 2 fixed, it might be useful to have a discrete distribution for both parents and have this discrete distribution updated according to where good candidate parents occur. This way, we try to approximate a distribution that wastes as little time considering bad candidate parents as possible. We know already that these distributions exist by empirical measures. 

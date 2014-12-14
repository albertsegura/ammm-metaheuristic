#include <iostream>
#include "SampleDecoder.h"
#include "myDecoder.cpp"
#include "MTRand.h"
#include "BRKGA.h"
#include <malloc.h>
#include <time.h>
#include <ctime>

using namespace std;

void print_chromosome(list<int> permutation);
list<int> get_chromosome_list(std::vector< double >& chromosome);

int main(int argc, char* argv[]) {
const unsigned n = 750;
	const unsigned p = 100; 	// size of population
	const double pe = 0.20;		// fraction of population to be the elite-set
	const double pm = 0.10;		// fraction of population to be replaced by mutants
	const double rhoe = 0.70;	// probability that offspring inherit an allele from elite parent
	const unsigned K = 3;		// number of independent populations
	const unsigned MAXT = 2;	// number of threads for parallel decoding
	
//ddavila:
   
    int maxiter=1000;
    int inst_num, proc_num;
    int *I;
    int *df, *ex, *wb;

//Read input
    cin >> proc_num;
    cin >> inst_num;

    if(inst_num!=n ){
        cout << "ERROR: Instruction number is fixed to: "<<n <<endl;
        exit(1); 
    }
    I=(int*)malloc(sizeof(int)*inst_num* inst_num);

    df=(int*)malloc(sizeof(int)*inst_num);
    ex=(int*)malloc(sizeof(int)*inst_num);
    wb=(int*)malloc(sizeof(int)*inst_num);

  
    for(int i=0; i< inst_num; i++){
        for(int j=0; j<inst_num; j++){
            cin >> I[i*inst_num+j];
        }
    }

    for(int i=0; i< inst_num; i++){
        cin >> df[i];
    }
    for(int i=0; i< inst_num; i++){
        cin >> ex[i];
    }
    for(int i=0; i< inst_num; i++){
        cin >> wb[i];
    }

    myDecoder my_decoder(inst_num, proc_num, I, df, ex, wb, maxiter);
//****************************************    
	SampleDecoder decoder(inst_num, proc_num, I, df, ex, wb, maxiter);			// initialize the decoder
	
    //ddavila
    srand (time(NULL));
    const long unsigned rngSeed = rand()%10;	// seed to the random number generator
//*******************************************


	//const long unsigned rngSeed = 5;	// seed to the random number generator
	MTRand rng(rngSeed);				// initialize the random number generator
	
	// initialize the BRKGA-based heuristic
	BRKGA< SampleDecoder, MTRand > algorithm(n, p, pe, pm, rhoe, decoder, rng, K, MAXT);
	
	unsigned generation = 0;		// current generation
	const unsigned X_INTVL = 100;	// exchange best individuals at every 100 generations
	const unsigned X_NUMBER = 2;	// exchange top 2 best
	const unsigned MAX_GENS = 1000;	// run for 1000 gens
	do {
		algorithm.evolve();	// evolve the population for one generation
		
		if((++generation) % X_INTVL == 0) {
			algorithm.exchangeElite(X_NUMBER);	// exchange top individuals
		}
	} while (generation < MAX_GENS);
	
    cout << algorithm.getBestFitness() << std::endl;
/*
	std::cout << "Best solution found has objective value = "
	 		<< algorithm.getBestFitness() << std::endl;
    std::vector< double > best_chromo=algorithm.getBestChromosome();
	std::cout << "With structure: "<<std::endl;
    list<int> best_chromosome_list;
    best_chromosome_list=get_chromosome_list(best_chromo);
    print_chromosome(best_chromosome_list);
    my_decoder.calculate_fitness_value(best_chromosome_list);
*/
	return 0;
}

/*ddavila*/
list<int> get_chromosome_list(std::vector< double >& chromosome){
	std::vector< std::pair< double, int > > ranking(chromosome.size());

	for(unsigned i = 0; i < chromosome.size(); ++i) {
		ranking[i] = std::pair< double, int >(chromosome[i], i);
	}

	// Here we sort 'permutation', which will then produce a permutation of [n] in pair::second:
	std::sort(ranking.begin(), ranking.end());

	// permutation[i].second is in {0, ..., n - 1}; a permutation can be obtained as follows
	list< int > permutation;
	for(std::vector< std::pair< double, int > >::iterator i = ranking.begin();
			i != ranking.end(); ++i) {
		permutation.push_back(i->second);
	}
   return permutation;
}
void print_chromosome(list<int> permutation){
    std::cout<< "chromosome: ";
    for(std::list<int>::iterator it = permutation.begin(); it != permutation.end(); it++){
        std::cout << *it << " ";
    }
    std::cout<< std::endl;
}

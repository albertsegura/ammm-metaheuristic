/*
 * myDecoder.h
 *
 * Any decoder must have the format below, i.e., implement the method decode(std::vector< double >&)
 * returning a double corresponding to the fitness of that vector. If parallel decoding is to be
 * used in the BRKGA framework, then the decode() method _must_ be thread-safe; the best way to
 * guarantee this is by adding 'const' to the end of decode() so that the property will be checked
 * at compile time.
 *
 * The chromosome inside the BRKGA framework can be changed if desired. To do so, just use the
 * first signature of decode() which allows for modification. Please use double values in the
 * interval [0,1) when updating, thus obeying the BRKGA guidelines.
 *
 *  Created on: Jan 14, 2011
 *      Author: rtoso
 */

#ifndef MYDECODER_H
#define MYDECODER_H

#include <list>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <malloc.h>
#include <string.h>


class myDecoder {
    int inst_num, proc_num;
    int *I;
    int *df, *ex, *wb;
    int *k_df, *k_ex, *k_wb;
    int *ci, *ei, *ti, *ti_end, *ki;
    int maxiter;

public:
	myDecoder(int Inst_num, int Proc_num, int *I_matrix, int *Df, int *Ex, int *W, int Maxiter);
	~myDecoder();

	double decode(const std::vector< double >& chromosome);
    double calculate_fitness_value(std::list<int>list_instruction);

private:
    void init_values();
    void init_predecesor_count();

    int find_best_k(int inst_id, int tdf, int tex, int twb, int procs, int *Sdf,
        int *Sex, int *Swb, int  ei, int *ti);
    void update_sucesors(int inst_id, int erliest_sucesors_time, int* ci, 
                int inst_num, int *I, int *ei);
    void print_counters(int inst_num, int *ci);
    int calculate_total_time(int inst_num, int *ti_end);
    void print_answer(int inst_num, int *ti, int *ki);
    void print_permutation(std::list<int>list_instruction);

};

#endif

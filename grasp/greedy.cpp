#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>
#include <functional>
#include <cstring>
//#include <string.h>

using namespace std;

int nproc;
int ninst;

float *stream_df;
float *stream_ex;
float *stream_wb;

struct inst_stages {
	float df;
	float ex;
	float wb;

	int num_predecessors;	// Ci <- |d-(i)|
	float start_time;	// Ei
};

int *inst_dep_matrix;
int *inst_pre_matrix;
int *inst_succ_matrix;

struct inst_stages *instructions;
priority_queue<int> inst_queue;


void print_input() {
	cout << endl << "Proc: " << nproc << endl;
	cout << "Inst: " << ninst << endl;
	cout << "Dep. Matrix: " << endl;
	for (int i=0; i<ninst*ninst; i++) {
		cout << inst_dep_matrix[i] << " ";
		if ((i+1)%ninst == 0) cout << endl;
	}

	cout << "Times: " << endl;
	for (int i=0; i<ninst; i++)
		cout << instructions[i].df << " ";
	cout << endl;
	for (int i=0; i<ninst; i++)
		cout << instructions[i].ex << " ";
	cout << endl;
	for (int i=0; i<ninst; i++)
		cout << instructions[i].wb << " ";
	cout << endl;
}

void print_output() {
	for (int k=0; k<nproc; k++) {

	}
}

int num_predecessors(int inst) {
	int num = 0;

	for (int i = 0; i < ninst; i++)
		num += inst_dep_matrix[i*ninst+inst];
	
	return num;
}


float h(int i, int k) {
	float a, b, c;
	
	a = max(instructions[i].start_time, stream_df[k]);
	b = max(a+instructions[i].df, stream_ex[k]);
	c = max(b+instructions[i].ex, stream_wb[k]);

	cout << endl << "start_time " << instructions[i].start_time << endl;
	cout << "stream df " << stream_df[k] << endl;
	cout << "stream ex " << stream_ex[k] << endl;
	cout << "stream wb " << stream_wb[k] << endl;
	cout << "a: " << a << " b: " << b << " c: " << c << endl << endl;

	return (c-(instructions[i].df+instructions[i].ex));
}

// TODO check for errors with the input2
// TODO save the streams
// TODO include start time and id of every instruction in the streams
// 

int main(int argc, char *argv[]) {

	/*  INPUT */
	cout << "Input number of processors: ";
	cin >> nproc;

	cout << "Input number of instructions: ";
	cin >> ninst;

	stream_df = (float *) malloc(sizeof(float)*nproc);
	stream_ex = (float *) malloc(sizeof(float)*nproc);
	stream_wb = (float *) malloc(sizeof(float)*nproc);

	memset(stream_df, 0, sizeof(float)*nproc);
	memset(stream_ex, 0, sizeof(float)*nproc);
	memset(stream_wb, 0, sizeof(float)*nproc);

	instructions = (struct inst_stages *) malloc(sizeof(struct inst_stages)*ninst);
	memset(instructions, 0, sizeof(struct inst_stages)*ninst);
	inst_dep_matrix = (int *) malloc(sizeof(struct inst_stages)*ninst*ninst);

	cout << "Input instruction dependency matrix: ";
	
	for (int i=0; i < ninst*ninst; i++) 
		cin >> inst_dep_matrix[i];
	
	for(int i=0; i < ninst; i++) 
		cin >> instructions[i].df;
	for(int i=0; i < ninst; i++)
		cin >> instructions[i].ex;
	for(int i=0; i < ninst; i++)
		cin >> instructions[i].wb;
	
	print_input();
	
	/* HEURISTIC */
	for (int inst=0; inst < ninst; inst++) {
		instructions[inst].num_predecessors = num_predecessors(inst);
		instructions[inst].start_time = 0;
		if (instructions[inst].num_predecessors == 0)
			inst_queue.push(inst);
	}

	while (!inst_queue.empty()) {
		int inst = inst_queue.top();
		inst_queue.pop();
		
		int minarg = 0;
		int minval = h(inst, 0);
		for (int i=1; i < nproc; i++) {
			int value = h(inst, i);
			if (minval > value) {
				minval = value;
				minarg = i;
			}
		}

		int k = minarg; // processor selected
		int start_time = minval; // start time 
		cout << "i: " << inst << " ti: " << start_time << endl;

		// TODO afegir a "llista" de inst processades per cada proc
		stream_df[k] = start_time+instructions[inst].df;
		stream_ex[k] = start_time+instructions[inst].df+instructions[inst].ex;
		stream_wb[k] = start_time+instructions[inst].df+instructions[inst].ex+instructions[inst].wb;

		cout << "Inst " << inst << " runs at proc: " << k << endl;
		cout << "\t" << stream_df[k] << " " << stream_ex[k] << " "  << stream_wb[k] << endl;
		
		for(int j=0; j < ninst; j++) {
			// if instruction j is a successor of inst
			if (inst_dep_matrix[inst*ninst+j]) {
				inst_dep_matrix[inst*ninst+j] = 0;
				instructions[j].num_predecessors--;
				instructions[j].start_time = max(instructions[j].start_time, 
						start_time+instructions[inst].df+instructions[inst].ex+instructions[inst].wb);

				if (instructions[j].num_predecessors == 0)
					inst_queue.push(j);
			}
		}
	}

	//print_output();

}

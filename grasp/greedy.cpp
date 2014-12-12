#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>
#include <functional>
#include <cstring>
#include <list>
#include <ctime>
//#include <string.h>

using namespace std;

int nproc;
int ninst;

struct inst_t {
	int ini_df, df, fini_df;
	int ini_ex, ex, fini_ex;
	int ini_wb, wb, fini_wb;

	int npredecessors;	// Ci <- |d-(i)|
	int start_time;		// Ei
	int id;
};

struct proc_t {
	list<int> sched_inst;
	int ninst;
};

int * inst_dep_matrix;
int * inst_npredecessors;
vector<struct proc_t> procs;
struct inst_t * instructions;

vector<struct proc_t> wtmp;
struct inst_t * winstructions;

class Compare {
	public:
	bool operator()(inst_t& i1, inst_t& i2) { // <
		if (i1.start_time <= i1.start_time) return true;
		return false;
	}
};

priority_queue<inst_t, std::vector<inst_t>, Compare> inst_queue;


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

int num_predecessors(int inst) {
	int num = 0;

	for (int i = 0; i < ninst; i++)
		num += inst_dep_matrix[i*ninst+inst];
	
	return num;
}

int hh(struct inst_t inst, int k, vector<struct proc_t> &proc, struct inst_t * vinst) {
	int a, b, c;
	int streamdf, streamex, streamwb;
	struct inst_t cpu_inst;

	if (proc[k].ninst == 0) {
		streamdf = 0;
		streamex = 0;
		streamwb = 0;
	} else {
		list<int>::iterator it = proc[k].sched_inst.begin();
		advance(it,proc[k].ninst-1);

		streamdf = vinst[*it].fini_df;
		streamex = vinst[*it].fini_ex;
		streamwb = vinst[*it].fini_wb;
	}

	a = max(inst.start_time, streamdf);
	b = max(a+inst.df, streamex);
	c = max(b+inst.ex, streamwb);

	//cout << endl << "start_time " << inst.start_time << endl;
	//cout << "stream df " << streamdf << endl;
	//cout << "stream ex " << streamex << endl;
	//cout << "stream wb " << streamwb << endl;
	//cout << "a: " << a << " b: " << b << " c: " << c << endl << endl;

	return (c-(inst.df+inst.ex));
}



int h(struct inst_t &inst, int k, struct inst_t * vinst) {
	int a, b, c;
	int streamdf, streamex, streamwb;
	struct inst_t cpu_inst;
	
	if (procs[k].sched_inst.empty()) {
		streamdf = 0;
		streamex = 0;
		streamwb = 0;
	}
	else {
		streamdf = vinst[procs[k].sched_inst.back()].fini_df;
		streamex = vinst[procs[k].sched_inst.back()].fini_ex;
		streamwb = vinst[procs[k].sched_inst.back()].fini_wb;
	}

	a = max(inst.start_time, streamdf);
	b = max(a+inst.df, streamex);
	c = max(b+inst.ex, streamwb);

	//cout << endl << "start_time " << inst.start_time << endl;
	//cout << "stream df " << streamdf << endl;
	//cout << "stream ex " << streamex << endl;
	//cout << "stream wb " << streamwb << endl;
	//cout << "a: " << a << " b: " << b << " c: " << c << endl << endl;

	return (c-(inst.df+inst.ex));
}

void init() {
	//cout << "Input number of processors: ";
	cin >> nproc;

	//cout << "Input number of instructions: ";
	cin >> ninst;

	// TODO
	srand(clock());
	//srand(1);

	procs.resize(nproc);
	wtmp.resize(nproc); 
	for (int i=0; i<nproc; i++) {
		procs[i].ninst = 0;
	}

	instructions = (struct inst_t *) calloc(ninst, sizeof(struct inst_t));
	winstructions = (struct inst_t *) calloc(ninst, sizeof(struct inst_t));
	inst_dep_matrix = (int *) malloc(sizeof(int)*ninst*ninst);
	inst_npredecessors = (int *) calloc(ninst, sizeof(int));

	//cout << "Input instruction dependency matrix: ";
	
	for (int i=0; i < ninst*ninst; i++) 
		cin >> inst_dep_matrix[i];
	
	for(int i=0; i < ninst; i++) 
		cin >> instructions[i].df;
	for(int i=0; i < ninst; i++)
		cin >> instructions[i].ex;
	for(int i=0; i < ninst; i++)
		cin >> instructions[i].wb;
}

void print_solution() {
	int i = 0;

	for (vector<proc_t>::iterator it = procs.begin() ; it != procs.end(); ++it, ++i) {
		cout << "Processor " << i << " , computes " << it->ninst << " instructions" << endl; 
		cout << "\t\tdf\tex\twb" << endl;
		for (list<int>::iterator itt = it->sched_inst.begin() ; itt != it->sched_inst.end(); ++itt) {
			cout << "\tInstruction " << instructions[*itt].id << ": "<< endl;
			cout << "\t\t" << instructions[*itt].ini_df << ", " << instructions[*itt].fini_df;
			cout << "\t\t" << instructions[*itt].ini_ex << ", " << instructions[*itt].fini_ex;
			cout << "\t\t" << instructions[*itt].ini_wb << ", " << instructions[*itt].fini_wb;
			cout << endl;
		}
	}
}

void rcl(struct inst_t inst, int *c_proc, int *c_start_time) {
	vector<int> rclv(nproc);
	vector<int> rclvid(nproc);

	int threshold, id, minarg, maxarg, minval, maxval, value, k, i;
	minarg = 0,
	maxarg = 0;
	minval = h(inst, 0, instructions);
	maxval = minval;
	//cout << minval << ", ";	
	
	rclv[0] = minval;
	for (k=1; k < rclv.size(); k++) {
		//cout << k << ", ";
		value = h(inst, k, instructions);
		//cout << value << ", ";
		rclv[k] = value;
		if (minval > value) {
			minval = value;
			minarg = k;
		}
		if (maxval < value) {
			maxval = value;
			maxarg = k;
		}
	}
	//cout << endl;
	//cout << "min: " << minval << " max: " << maxval << endl;
	threshold = minval+int(0.5*float(maxval-minval));
	//cout << "threshold: " << threshold << endl;

	//cout << threshold << ", " << rclv.size() << endl;
	/*for (int i=0; i<nproc; i++)
		cout << rclv[i] << ", ";
	cout << endl; 
	*/

	for (i=0, id=0; i < nproc; i++) {
		if (rclv[i] <= threshold) {
			rclv[id] = rclv[i];
			rclvid[id] = i;
			id++;
		}
	}

	*c_proc = rclvid[id!=0 ? rand()%id:0];

	*c_start_time = rclv[*c_proc];
}

void grasp(int greedy) {

	for (int inst=0; inst < ninst; inst++) {
		instructions[inst].id=inst;
		instructions[inst].start_time = 0;
		instructions[inst].ini_df = 0;
		instructions[inst].ini_ex = 0;
		instructions[inst].ini_wb = 0;
		instructions[inst].fini_df = 0;
		instructions[inst].fini_ex = 0;
		instructions[inst].fini_wb = 0;
		instructions[inst].npredecessors = num_predecessors(inst);
		inst_npredecessors[inst] = instructions[inst].npredecessors;
		if (instructions[inst].npredecessors == 0)
			inst_queue.push(instructions[inst]);
	}

	while (!inst_queue.empty()) {
		struct inst_t inst = inst_queue.top();
		inst_queue.pop();
		int k;
		int start_time;
		
		if (greedy) {
			int minarg = 0;
			int minval = h(inst, 0, instructions);
			for (k=1; k < nproc; k++) {
				int value = h(inst, k, instructions);
				if (minval > value) {
					minval = value;
					minarg = k;
				}
			}
			k=minarg;
			start_time = minval;
		} else {
			rcl(inst, &k, &start_time);
			//cout << k << ", " << start_time << endl;
		}

		instructions[inst.id].start_time = start_time;
		instructions[inst.id].ini_df  = start_time;
		instructions[inst.id].fini_df = start_time+inst.df;

		instructions[inst.id].ini_ex  =  start_time+inst.df;
		instructions[inst.id].fini_ex = start_time+inst.df+inst.ex;

		instructions[inst.id].ini_wb  = start_time+inst.df+inst.ex;
		instructions[inst.id].fini_wb = start_time+inst.df+inst.ex+inst.wb;

		procs[k].sched_inst.push_back(inst.id);
		procs[k].ninst++;
		
		//cout << "Inst " << inst.id << " runs at proc: " << k << endl;
		//cout << "\t" << inst.fini_df << " " << inst.fini_ex << " "  << inst.fini_wb << endl;
		
		//cout << "Successors for: " << inst.id << endl;
		for(int j=0; j < ninst; j++) {
			// if instruction j is a successor of inst
			if (inst_dep_matrix[inst.id*ninst+j]) {
				//cout << "dependecy with: " << j << endl;
				instructions[j].npredecessors--;
				
				instructions[j].start_time = max(instructions[j].start_time, 
						instructions[inst.id].fini_wb);
				//cout << "startime: " << instructions[j].start_time << " old: " << instructions[j].start_time << "inst" << instructions[inst.id].fini_wb << endl;

				if (instructions[j].npredecessors == 0)
					inst_queue.push(instructions[j]);
			}
		}
	}


}


int compute_times_local() {
	int proc_finished[nproc];
	int feasible = 1;
	int proc_blocked = 0;
	int nproc_finished = 0;

	for (int p=0; p<nproc; p++) 
		proc_finished[p] = 0;

	//cout << "Feasibility" << endl;
	for (int p=0; (proc_blocked + nproc_finished) < wtmp.size();) {
		// No instruction on p to reschedule
		//cout << "p:"<< p << " proc blocked: " << proc_blocked << ", proc finished: " << nproc_finished<< endl;
		if(wtmp[p].ninst == wtmp[p].sched_inst.size()) {
			if (!proc_finished[p]) nproc_finished++;
			proc_finished[p] = 1;
			p=(p+1)%wtmp.size();
			continue;
		}

		list<int>::iterator it = wtmp[p].sched_inst.begin();
		advance(it,wtmp[p].ninst);
		
		struct inst_t * inst = &winstructions[*it];

		// If instruction has no predecessors, schedule it
		//cout << "\tInst id: " << inst->id << ", predecessors: " << inst->npredecessors << endl;
		if (inst->npredecessors > 0) {
			proc_blocked++;
			p=(p+1)%wtmp.size();
			continue;
		} else {
			proc_blocked = 0;
			int start_time =  hh(*inst, p, wtmp, winstructions);

			winstructions[inst->id].start_time = start_time;
			winstructions[inst->id].ini_df  = start_time;
			winstructions[inst->id].fini_df = start_time+inst->df;

			winstructions[inst->id].ini_ex  =  start_time+inst->df;
			winstructions[inst->id].fini_ex = start_time+inst->df+inst->ex;

			winstructions[inst->id].ini_wb  = start_time+inst->df+inst->ex;
			winstructions[inst->id].fini_wb = start_time+inst->df+inst->ex+inst->wb;

			wtmp[p].ninst++;
		}

		// Update predecessors
		for(int j=0; j < ninst; j++) {
			// if instruction j is a successor of inst
			if (inst_dep_matrix[inst->id*ninst+j]) {
				//cout << "\t dep between: " << inst->id << "," << j << endl;
				winstructions[j].npredecessors--;
				winstructions[j].start_time = max(winstructions[j].start_time, 
						winstructions[inst->id].fini_wb);

			}
		}


	}

	if (proc_blocked > 0) feasible = 0;


	return feasible;
}

void copy_solution( vector<struct proc_t> &dest_proc, vector<struct proc_t> &source_proc, 
		    struct inst_t * dest_inst, struct inst_t * source_inst ) {

	for (int i=0; i<dest_proc.size(); i++) {
		dest_proc[i].ninst = source_proc[i].ninst;
		dest_proc[i].sched_inst.clear();
		dest_proc[i].sched_inst.insert(
			dest_proc[i].sched_inst.begin(), 
			source_proc[i].sched_inst.begin(), 
			source_proc[i].sched_inst.end());
	}
	
	for(int i=0; i<ninst; i++) {
		dest_inst[i] = source_inst[i];
	}
}

void clean_solution(vector<struct proc_t> &proc, struct inst_t * inst) {
	//cout << "clean solution" << endl;
	for(int i=0; i<proc.size(); i++)
		proc[i].ninst = 0;

	for(int i=0; i<ninst; i++) {
		inst[i].ini_df = 0;
		inst[i].fini_df = 0;
		inst[i].ini_ex = 0;
		inst[i].fini_ex = 0;
		inst[i].ini_wb = 0;
		inst[i].fini_wb = 0;
		inst[i].npredecessors = inst_npredecessors[i];
		inst[i].start_time = 0;
	}

}

int evaluate_solution(vector<struct proc_t> &proc_sol, struct inst_t * inst_sol) {
	//cout << "evaluate_solution" << endl;
	int time = 0;
	for (vector<proc_t>::iterator it = procs.begin() ; it != procs.end(); ++it) {
		time = max(time, inst_sol[it->sched_inst.back()].fini_wb);
	}
	
	return time;
}


void exchange_inst(int p, int pp, int i1, int i2) {
	list<int>::iterator itp = wtmp[p].sched_inst.begin();
	advance(itp,i1);

	list<int>::iterator itpp = wtmp[pp].sched_inst.begin();
	advance(itpp,i2);

	int aux = *itp;
	*itp = *itpp;
	*itpp = aux;
}

void local_search() {
	int feasible;
	list<int>::iterator it_inst1, it_inst2;
	copy_solution(wtmp, procs, winstructions, instructions); // wtmp <- w

	for (int p=0; p<nproc; p++) {
		//cout << "p: " << p << endl;
	for (int pp=p+1; pp<nproc; pp++) {
		//cout << "pp: " << pp << endl;
		for (int i1=0; i1<wtmp[p].sched_inst.size(); i1++) {
			//cout << "inst1: " << i1 << "/" << wtmp[p].sched_inst.size() << endl;
		for (int i2=0; i2<wtmp[pp].sched_inst.size(); i2++) {
			//cout << "inst2: " << i2 << "/" << wtmp[pp].sched_inst.size() << endl;

			// Exchange
			exchange_inst(p, pp, i1, i2);

			// Clean instruction tmp solution
			clean_solution(wtmp, winstructions);
			
			// Compute times with the proposed solution
			feasible = compute_times_local();
			//if (feasible) cout << "Feasible: " << evaluate_solution(wtmp, winstructions) << endl;

			// If feasible and better it becomes the new solution
			if ( feasible && (evaluate_solution(wtmp, winstructions) < evaluate_solution(procs, instructions)) ) {
				copy_solution(procs, wtmp, instructions, winstructions); // w <- wtmp
			}
			else
				copy_solution(wtmp, procs, winstructions, instructions); // wtmp <- w
			//cout << "Next" << endl;
		}
		}
	}
	}
}

int main(int argc, char *argv[]) {

	init();

	//print_input();
	
	grasp(0);


	//print_solution();
	//cout << "Initial solution: " << evaluate_solution(procs, instructions) << endl;

	local_search();

	//cout << "Final solution: " << evaluate_solution(procs, instructions) << endl;

	//print_solution();
	cout << evaluate_solution(procs, instructions) << endl;

}

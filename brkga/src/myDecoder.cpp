
#include "myDecoder.h"


using namespace std;

myDecoder::myDecoder(int Inst_num, int Proc_num, int *I_matrix, int *Df, int *Ex, int *Wb, int Maxiter) {

    inst_num=Inst_num;
    proc_num=Proc_num;
    maxiter=Maxiter;

    I=I_matrix;

    df=Df;
    ex=Ex;
    wb=Wb;
    
    //ddavila: sizeof proc_num
    k_df=(int*)malloc(sizeof(int)*proc_num);
    k_ex=(int*)malloc(sizeof(int)*proc_num);
    k_wb=(int*)malloc(sizeof(int)*proc_num);

    //ddavila: sizeof inst_num
    ci=(int*)malloc(sizeof(int)*inst_num);
    ei=(int*)malloc(sizeof(int)*inst_num);

    ti=(int*)malloc(sizeof(int)*inst_num);
    ti_end=(int*)malloc(sizeof(int)*inst_num);
    ki=(int*)malloc(sizeof(int)*inst_num);

    //init_predecesor_count();
 }

myDecoder::~myDecoder() { }

// Runs in \Theta(n \log n):
double myDecoder::decode(const std::vector< double >& chromosome){
	std::vector< std::pair< double, int > > ranking(chromosome.size());

	for(unsigned i = 0; i < chromosome.size(); ++i) {
		ranking[i] = std::pair< double, int >(chromosome[i], i);
	}

	// Here we sort 'permutation', which will then produce a permutation of [n] in pair::second:
	std::sort(ranking.begin(), ranking.end());

	// permutation[i].second is in {0, ..., n - 1}; a permutation can be obtained as follows
	std::list< int > permutation;
	for(std::vector< std::pair< double, int > >::iterator i = ranking.begin();
			i != ranking.end(); ++i) {
		permutation.push_back(i->second);
	}
   
    
    int t=calculate_fitness_value(permutation);   

    return  t;
}

double myDecoder::calculate_fitness_value(std::list<int>list_instruction){

    int best_k=-1;
    int inst_id;
    int total_time;
    int current_ti;
    int erliest_sucesors_time;

    int end=0;
    int iter=0;
    list<int>::iterator it;
    
    init_values();
    init_predecesor_count();

    while(end==0){
        for(it=list_instruction.begin(); it!=list_instruction.end(); it++){
            if(ci[*it]==0){   
                best_k=find_best_k(*it, df[*it], ex[*it], wb[*it],
                        proc_num, k_df, k_ex, k_wb, ei[*it], &current_ti);
                
                k_df[best_k]=current_ti+df[*it];
                k_ex[best_k]=current_ti+df[*it]+ex[*it];
                k_wb[best_k]=current_ti+df[*it]+ex[*it]+wb[*it];
                erliest_sucesors_time=current_ti+df[*it]+ex[*it]+wb[*it];
                //print_counters(inst_num, ci);
                update_sucesors(*it, erliest_sucesors_time, ci, inst_num, I, ei); 
                cout<< "Inst: " << *it << ", kbest: " << best_k << ", ti: " << current_ti <<endl; 
                ti[*it]=current_ti;
                ti_end[*it]=current_ti+df[*it]+ex[*it]+wb[*it];
                ki[*it]=best_k;
                list_instruction.erase(it);
                //print_counters(inst_num, ci);
                break;
            }
        }
        iter++;
        //End conditions
        if(iter==maxiter || list_instruction.empty()){
            end=1;
        }
    }
    print_answer(inst_num, ti, ki);
    total_time=calculate_total_time(inst_num, ti_end);
    cout << "Total time: " << total_time  << endl;   
    return total_time;
}

void myDecoder::print_answer(int inst_num, int *ti, int *ki){
    int i,j, counter;
    for(i=0; i< inst_num; i++){
        cout << ">Inst: " << i << "-> k: " << ki[i] <<", ti: "<<ti[i]<<endl;
    }
}

int myDecoder::calculate_total_time(int inst_num, int *ti_end){
    int i;
    int tmax=0;
    for(i=0; i< inst_num; i++){
        if(ti_end[i]>tmax){
            tmax=ti_end[i];
        }
    }
    return tmax;
}

void myDecoder::update_sucesors(int inst_id, int erliest_sucesors_time, int* ci, 
            int inst_num, int *I, int *ei){
    int i,j, counter;

    for(i=0; i< inst_num; i++){
        if(I[i+inst_num*inst_id]==1){
            ci[i]=ci[i]-1;
            ei[i]=max(ei[i],erliest_sucesors_time);
        }        
    }
}

//function h in paper
int myDecoder::find_best_k(int inst_id, int tdf, int tex, int twb, 
                int procs, int *Sdf, int *Sex, int *Swb, int  ei, int *ti){
    
    int k, erliest, erliest_index, erliest_min;
    erliest_index=0;
    //CHECK THIS
    erliest_min=ei+Swb[0];
    for(k=0; k<procs; k++){
        erliest= max(max( max(ei, Sdf[k])+tdf, Sex[k])+tex, Swb[k])-(tdf+tex);
        if(erliest<erliest_min){
            erliest_min=erliest;
            erliest_index=k;
        }
    }
    *ti=erliest_min;
    //cout<< "*****" << erliest_min << " index: " << erliest_index <<endl;
    return erliest_index;
}

void myDecoder::print_counters(int inst_num, int *ci){
   //Print counters
    int i, j;
    cout << "--------Counters---------" <<endl;
    for(i=0; i< inst_num; i++){
        for(j=0; j< inst_num; j++){
            cout << "Inst: " << i << ", counter: " << ci[i] <<endl;
        }
    }
    cout << "-------------------------" <<endl;
}

void myDecoder::init_values(){
    memset(ei, 0, sizeof(int)*inst_num);  
    memset(k_df, 0, sizeof(int)*proc_num);  
    memset(k_ex, 0, sizeof(int)*proc_num);  
    memset(k_wb, 0, sizeof(int)*proc_num);  

    memset(ti, -1, sizeof(int)*inst_num);  
    memset(ti_end, -1, sizeof(int)*inst_num);  
    memset(ki, -1, sizeof(int)*inst_num);  
    
}

//Initialize predecesor counters
void myDecoder::init_predecesor_count(){
    int i,j, counter;
    for(i=0; i< inst_num; i++){
        counter=0;
        for(j=0; j< inst_num; j++){
            counter+=I[i+inst_num*j];
        }
        ci[i]=counter;
    }
}
void myDecoder::print_permutation(std::list<int>list_instruction){
    list<int>::iterator it;
    for(it=list_instruction.begin(); it!=list_instruction.end(); it++){
        cout << *it << " ";
    }
}



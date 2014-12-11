#include <iostream>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

bool debug;

class RandomNumberGenerator {
	private:
		int max;

	public:
		RandomNumberGenerator(int max) {
			this->max = max;
			srand(time(NULL));
		}

		~RandomNumberGenerator() {}

		int random() {
			return rand() % this->max;
		}

		int random(int max) {
			return rand() % max;
		}
};

class Instruction {
	public:
		int id;
		int timeFD;
		int timeEX;
		int timeWB;
		int parent1;
		int parent2;
		int numInstructions;

		Instruction() {}

		~Instruction() {}

		void setInstruction(int id, int tFD, int tEX, int tWB, int p1, int p2, int numInstructions) {
			this->id = id;
			timeFD = tFD;
			timeEX = tEX;
			timeWB = tWB;
			parent1 = p1;
			parent2 = p2;
			this->numInstructions = numInstructions;
		}

		void setDependencies(int * dependencyMatrix) {
			if (parent1 != id)
				dependencyMatrix[(parent1*numInstructions)+id] = 1;
			if (parent2 != id)
				dependencyMatrix[(parent2*numInstructions)+id] = 1;
		}
};

class InputGenerator {
	public:
		int numProcs;
		int numInstructions;
		Instruction * instructions;
		int * dependencyMatrix;
		RandomNumberGenerator * rng;

	public:
		InputGenerator(int numProcs, int numInstructions) {
			this->numProcs = numProcs;
			this->numInstructions = numInstructions;

			instructions = new Instruction[numInstructions];
			dependencyMatrix = new int[numInstructions*numInstructions];
			rng = new RandomNumberGenerator(10);
			generateInstructions();
			generateMatrix();
		}

		~InputGenerator() {
			delete[] dependencyMatrix;
			delete[] instructions;
			delete rng;
		}

		void generateInstructions() {
			for (int i = 0; i < numInstructions; ++i) {
				instructions[i].timeFD = rng->random(10);
				instructions[i].timeEX = rng->random(10);
				instructions[i].timeWB = rng->random(10);

				int predecesors = rng->random(3);
				if (predecesors == 1 && i >= 1) {
					instructions[i].parent1 = rng->random(i-1);
					instructions[i].parent2 = -1;
				} else if (predecesors == 2 && i >= 2) {
					instructions[i].parent1 = rng->random(i-1);
					instructions[i].parent2 = rng->random(i-1);
					if (instructions[i].parent1 == instructions[i].parent2)
						instructions[i].parent2 = (instructions[i].parent2+1)%(i-1);
				} else {
					instructions[i].parent1 = -1;
					instructions[i].parent2 = -1;
				}
			}
		}

		void generateMatrix() {
			for (int i = 0; i < numInstructions; ++i) {
				if (instructions[i].parent1 != -1)
					dependencyMatrix[(instructions[i].parent1*numInstructions)+i] = 1;
				if (instructions[i].parent2 != -1)
					dependencyMatrix[(instructions[i].parent2*numInstructions)+i] = 1;
			}
		}

		string getMatrixILP() {
			string s = "\n[";
			for (int i = 0; i < numInstructions+1; ++i) {
				s = s + "0 ";
			}
			s = s + "]\n";
			for (int i = 0; i < numInstructions; ++i) {
				s = s + "[ 0 ";
				for (int j = 0; j < numInstructions; ++j) {
					s = s + to_string(dependencyMatrix[(i*numInstructions)+j]) + " ";
				}
				s = s + "]\n";
			}
			return s;
		}

		string getMatrixMH() {
			string s;
			for (int i = 0; i < numInstructions; ++i) {
				for (int j = 0; j < numInstructions; ++j) {
					s = s + to_string(dependencyMatrix[(i*numInstructions)+j]) + " ";
				}
				s = s + "\n";
			}
			return s;
		}

		string getFDvector() {
			string s;
			for (int i = 0; i < numInstructions; ++i) {
				s = s + to_string(instructions[i].timeFD) + " ";
			}
			return s;
		}

		string getEXvector() {
			string s;
			for (int i = 0; i < numInstructions; ++i) {
				s = s + to_string(instructions[i].timeEX) + " ";
			}
			return s;
		}

		string getWBvector() {
			string s;
			for (int i = 0; i < numInstructions; ++i) {
				s = s + to_string(instructions[i].timeWB) + " ";
			}
			return s;
		}

		string getFDvectorILP() {
			string s = "0 ";
			for (int i = 0; i < numInstructions; ++i) {
				s = s + to_string(instructions[i].timeFD) + " ";
			}
			return s;
		}

		string getEXvectorILP() {
			string s = "0 ";
			for (int i = 0; i < numInstructions; ++i) {
				s = s + to_string(instructions[i].timeEX) + " ";
			}
			return s;
		}

		string getWBvectorILP() {
			string s = "0 ";
			for (int i = 0; i < numInstructions; ++i) {
				s = s + to_string(instructions[i].timeWB) + " ";
			}
			return s;
		}

		void writeILP(string name) {
			string s = getILPstring();
			writeToFile(name, s);
		}

		void writeMH(string name) {
			string s = getMHstring();
			writeToFile(name, s);
		}

		void writeToFile(string name, string text) {
			ofstream myfile;
			myfile.open (name);
			myfile << text;
			myfile.close();
		}

		string getILPstring() {
			string ilp1 = "nInstructions = ";
			string ilp2 = ";\np = ";
			string ilp3 = ";\nE = [";
			string ilp4 = "];\ntdf = [";
			string ilp5 = "];\ntex = [";
			string ilp6 = "];\ntwb = [";
			string ilp7 = "];\n";

			string d1 = to_string(numInstructions+1);
			string d2 = to_string(numProcs);

			string m = getMatrixILP();

			string vFD = getFDvectorILP();
			string vEX = getEXvectorILP();
			string vWB = getWBvectorILP();

			string final = ilp1+d1+ilp2+d2+ilp3+m+ilp4+vFD+ilp5+vEX+ilp6+vWB+ilp7;

			return final;
		}

		string getMHstring() {
			string d1 = to_string(numInstructions);
			string d2 = to_string(numProcs);

			string m = getMatrixMH();

			string vFD = getFDvector();
			string vEX = getEXvector();
			string vWB = getWBvector();

			string final = d1+"\n"+d2+"\n\n"+m+"\n"+vFD+"\n"+vEX+"\n"+vWB+"\n";

			return final;
		}

};

string convert(int num, int digits) {
	string s = to_string(num);
	int sDigits = s.size();
	for (int i = 0; i < digits-sDigits; ++i)
		s = "0"+s;
	return s;
}

int main(int argc, const char* argv[]) {
	if (argc != 6) {
		cout << "usage: InputGenerator numProcs numInstructions inputNum numDigits debug" << endl;
		return EXIT_FAILURE;
	}
	string num = convert(atoi(argv[3]), atoi(argv[4]));
	debug = atoi(argv[5]) == 1 ? true : false;
	InputGenerator * ig = new InputGenerator(atoi(argv[1]), atoi(argv[2]));
	if (debug) {
		cout << "Generating " << ig->numInstructions << " random instructions." << endl;
	}
	ig->writeILP("Input"+num+".dat");
	ig->writeMH("Input"+num+".mh");
	delete ig;
	return EXIT_SUCCESS;
}


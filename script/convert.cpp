#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
using namespace std;

int main(int argc, const char* argv[]) {
	if (argc != 3) {
		cout << "usage: convert num digits" << endl;
		return EXIT_FAILURE;
	}
	int num = atoi(argv[1]);
	int digits = atoi(argv[2]);
	string s = to_string(num);
	int sDigits = s.size();
	for (int i = 0; i < digits-sDigits; ++i)
		s = "0"+s;
	cout << s;
	return EXIT_SUCCESS;
}

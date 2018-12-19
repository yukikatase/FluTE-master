#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
using namespace std;
int main(){
	string line1 = "";
	string line2 = "";
	ofstream outputfile("test.text");
	ifstream inputfile1("tes.text");
	vector<string> s1;
	int i = 0;

	while(getline(inputfile1, line1)){
	  	stringstream ss;
	  	ss << line1;

	  	while(getline(ss, line2, ' ')){
	  		s1.push_back(line2);
	  		outputfile << s1[i++] << endl;
	  	}
  	}

  	inputfile1.close();
	outputfile.close();
	return 0;
}
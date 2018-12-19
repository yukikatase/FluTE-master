#include <string>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
int main(){
  string line1 = "";
  ofstream outputfile("result.text");
  ifstream inputfile1("RecoveredFromHospital.txt");
  vector<string> s;

  int hospital[5][2] = {};
  int hospitalduration = 0;
  int i = 0;

  while(getline(inputfile1, line1)){
  	s.push_back(line1);
  	outputfile << s.size() << endl;
  	outputfile << s[i++];
  	outputfile << endl;
  }

  for (int i = 0; i < s.size()/3; ++i){
  	hospital[stoi(s[0 + i*3])][stoi(s[1 + i*3])]++;
    if(stoi(s[0 + i*3])==2 || stoi(s[0 + i*3])==3){
      hospitalduration += stoi(s[2 + i*3]);
    }
  }

  outputfile << hospitalduration << endl;

  for (int i = 0; i < 2; ++i){
    for (int j = 0; j < 5; ++j){
      outputfile << j << " " << i << " " << hospital[j][i] << endl;
    }
  }

  ifstream inputfile2("Dead.txt");
  getline(inputfile2, line1);

  inputfile1.close();
  inputfile2.close();
  outputfile.close();
  return 0;
}
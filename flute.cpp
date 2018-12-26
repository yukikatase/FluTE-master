/* main program for flute
 *
 * Dennis Chao
 * 12.2008
 */

#ifdef PARALLEL
#include <mpi.h>
#include "epimodel.h"
#include "epimodelparameters.h"

using namespace std;
int main(int argc, char *argv[]) {
  MPI::Init();
  int rank = MPI::COMM_WORLD.Get_rank();
  int size = MPI::COMM_WORLD.Get_size();
  char *configname=NULL;
  if (argc==2)
    configname = argv[1];
  else {
    if (!rank)
      cerr << "Usage: " << argv[0] << " configfilename" << endl;
    MPI::Finalize();
    exit(-1);
  }
  EpiModelParameters parms(rank, size, configname);
  EpiModel model(rank, size, parms);
  model.run();
  MPI::Finalize();
  return 0;
}
#else // single processor mode
#include "epimodel.h"
#include "epimodelparameters.h"
#include <string>

vector<string> tovector2(string line1){
  string line;
  vector<string> s1;
  vector<string> s2;

  stringstream ss;
  ss << line1;

  while(getline(ss, line, ' ')){
    s1.push_back(line);
  }

  s2.insert(s2.end(), s1.begin(), s1.end());
  s1.clear();
  return s2;
}

void configRename(string R0, string schoolclosuredays, string isolation, string quarantine, string quarantinedays, string schoolclosurestudents, string seed){
  ifstream inputfile("config-twodose");
  string line;
  vector<string> s1; //5列 Dead
  while(getline(inputfile, line)){
    vector<string> s2;
    s2 = tovector2(line);
    if(s2[0]=="R0"){
      s2[1]=R0;
    }
    if(s2[0]=="schoolclosuredays"){
      s2[1]=schoolclosuredays;
    }
    if(s2[0]=="isolation"){
      s2[1]=isolation;
    }
    if(s2[0]=="quarantine"){
      s2[1]=quarantine;
    }
    if(s2[0]=="schoolclosurestudents"){
      s2[1]=schoolclosurestudents;
    }
    if(s2[0]=="seed"){
      s2[1]=seed;
    }
    s1.insert(s1.end(), s2.begin(), s2.end());
  }
  inputfile.close();

  ofstream outputfile("config-twodose");
  while(!s1.empty()){
    if(s1[0]=="preexistingimmunitybyage" || s1[0]=="highriskfraction"){
      outputfile<<s1[0]<<" "<<s1[1]<<" "<<s1[2]<<" "<<s1[3]<<" "<<s1[4]<<" "<<s1[5]<<endl;
      for (int i = 0; i < 6; ++i){
        s1.erase(s1.begin());
      }

    }else if(s1[0]=="vaccinedata"){
      outputfile<<s1[0]<<" "<<s1[1]<<" "<<s1[2]<<" "<<s1[3]<<" "<<s1[4]<<" "<<s1[5]<<" "<<s1[6]<<" "<<s1[7]<<" "<<s1[8]<<" "<<s1[9]<<" "<<s1[10]<<" "<<s1[11]<<endl;
      for (int i = 0; i < 12; ++i){
        s1.erase(s1.begin());
      }

    }else{
      outputfile<<s1[0]<<" "<<s1[1]<<endl;
      s1.erase(s1.begin());
      s1.erase(s1.begin());
    }
  }
  outputfile.close();
}

int main() {
  char *configname=NULL;
  int argc = 2;
  char argv[] = "config-twodose";

  if (argc==2)
    configname = argv;
  else {
    cerr << "Usage: " << argv[0] << " configfilename" << endl;
    exit(-1);
  }
  ofstream outputfile1("Quarantined.txt", ios::trunc);
  outputfile1.close();
  ofstream outputfile2("Withdrawed.txt", ios::trunc);
  outputfile2.close();
  ofstream outputfile5("Recovered.txt", ios::trunc);
  outputfile5.close();
  ofstream outputfile6("clearQuarantined.txt", ios::trunc);
  outputfile6.close();
  ofstream outputfile7("RecoveredFromHospital.txt", ios::trunc);
  outputfile7.close();
  ofstream outputfile8("Dead.txt", ios::trunc);
  outputfile8.close();
  ofstream outputfile9("School.txt", ios::trunc);
  outputfile9.close();
  double r0[2]={1.5,2.0};
  int schoolclosuredays[2]={3,5};
  double isolation[2]={0.8,0.95};
  double quarantine[2]={0.3,0.5};
  int quarantinedays[2]={3,5};
  int schoolclosurestudents[2]={10,15};
  int seed[10]={1,2,3,4,5,6,7,8,9,10};

  EpiModelParameters parms(configname);
  EpiModel model(parms);
  model.run();
  //configRename(to_string(r0[1]),to_string(schoolclosuredays[1]),to_string(isolation[1]),to_string(quarantine[1]),to_string(quarantinedays[1]),to_string(schoolclosurestudents[1]),to_string(seed[1]));
  return 0;
}

#endif

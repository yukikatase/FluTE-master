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


int result(){
  string line = "";
  ofstream outputfile("result.text");
  ifstream inputfile1("RecoveredFromHospital.txt");
  int hospital[5][2] = {};
  int hospitalduration = 0;

  while(getline(inputfile1, line, ' ')){
    hospital[(int)line[0]][(int)line[1]]++;
    if((int)line[0]==2 || (int)line[0]==3){
      hospitalduration += (int)line[3];
    }
    outputfile << line << endl;
  }

  for (int i = 0; i < 2; ++i){
    for (int j = 0; j < 5; ++j){
      outputfile << hospital[j][i] << endl;
    }
  }

  ifstream inputfile2("Dead.txt");
  getline(inputfile2, line);

  inputfile1.close();
  inputfile2.close();
  outputfile.close();
  return 0;
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

  EpiModelParameters parms(configname);
  EpiModel model(parms);
  model.run();
  // result();
  return 0;
}

#endif

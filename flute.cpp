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

int main(int argc, char *argv[]) {
  char *configname=NULL;
  if (argc==2)
    configname = argv[1];
  else {
    cerr << "Usage: " << argv[0] << " configfilename" << endl;
    exit(-1);
  }
  ofstream outputfile1("Quarantined.txt", ios::trunc);
  outputfile1.close();
  ofstream outputfile2("Withdrawed.txt", ios::trunc);
  outputfile2.close();
  ofstream outputfile3("Recoverd.txt", ios::trunc);
  outputfile3.close();
  ofstream outputfile4("clearQuarantined.txt", ios::trunc);
  outputfile4.close();
  ofstream outputfile5("RecoverdFromHospital.txt", ios::trunc);
  outputfile5.close();
  EpiModelParameters parms(configname);
  EpiModel model(parms);
  model.run();
  return 0;
}
#endif

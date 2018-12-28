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
vector<string> tovector(string line1){
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

vector<vector<string> > queue(int i, vector<string> s2, vector<string> s3){
  vector<vector<string> > a;
  for (int j = i+1; j < s2.size()/5; ++j){
    if(s2[0+i*5]==s2[0+j*5] && stoi(s2[2+i*5])+stoi(s2[3+i*5]) > stoi(s2[2+j*5])){
      s2[3+i*5] = to_string(stoi(s2[2+j*5]) + stoi(s2[3+j*5]) - stoi(s2[2+i*5]));

      s2.erase(s2.begin() + j*5);
      s2.erase(s2.begin() + j*5);
      s2.erase(s2.begin() + j*5);
      s2.erase(s2.begin() + j*5);
      s2.erase(s2.begin() + j*5);
      a = queue(i, s2, s3);
      break;
    }else if(stoi(s2[2+i*5])+stoi(s2[3+i*5]) > stoi(s2[2+j*5]) + 1){
      break;
    }
  }
  if(a.empty()){
    s3.push_back(s2[0+i*5]);
    s3.push_back(s2[1+i*5]);
    s3.push_back(s2[2+i*5]);
    s3.push_back(s2[3+i*5]);
    s3.push_back(s2[4+i*5]);
    a.push_back(s2);
    a.push_back(s3);
  }
  return a;
}


void qua(string infile, string outfile){
  ifstream inputfile1(infile);
  vector<string> s2;
  vector<string> s3;
  string line;
  while(getline(inputfile1, line)){
    vector<string> s1;
    s1 = tovector(line);
    s2.insert(s2.end(), s1.begin(), s1.end());
  }
  ofstream outputfile(outfile, ios::trunc);
  vector<vector<string> > a;

  for (int i = 0; i < s2.size()/5; ++i){
    a = queue(i, s2, s3);
    s2 = a[0];
    s3 = a[1];
  }

  for (int i = 0; i < s3.size()/5; ++i){
    outputfile<<s3[0+i*5]<<" "<<s3[1+i*5]<<" "<<s3[2+i*5]<<" "<<s3[3+i*5]<<" "<<s3[4+i*5]<<endl;
  }
  inputfile1.close();
  outputfile.close();
}

void withdraw(string infile1, string infile2, string infile3, string infile4, string outfile){
  string line;

  ifstream inputfile1(infile1);
  vector<string> s1; //5列 quarantined
  while(getline(inputfile1, line)){
    vector<string> s3;
    s3 = tovector(line);
    s1.insert(s1.end(), s3.begin(), s3.end());
    s3.clear();
    s3.shrink_to_fit();
  }
  inputfile1.close();

  ifstream inputfile2(infile2);
  vector<string> s2; //6列 recovered
  while(getline(inputfile2, line)){
    vector<string> s3;
    s3 = tovector(line);
    if(s3[1]=="1"){
      s2.insert(s2.end(), s3.begin(), s3.end());
    }
    s3.clear();
    s3.shrink_to_fit();
  }
  inputfile2.close();
  cout<<"with before recover"<<endl;
  int insert2;
  int l;
  int withstart;
  int withlast;
  int quastart;
  int qualast;

  for (int i = 0; i < s2.size()/6; ++i){// Reover
    insert2=s1.size()/5-1;
    l=0;
    for (int j = 0; j < s1.size()/5; ++j){
      if(stoi(s1[2+5*j])==stoi(s2[3+6*i])-stoi(s2[2+6*i])+1 && l==0){
        insert2=j;
        l++;
      }
      if(stoi(s1[2+5*j])>stoi(s2[3+6*i])-stoi(s2[2+6*i])+1 && l==0){
        insert2=j;
        l++;
      }
      if(stoi(s2[0+6*i]) == stoi(s1[0+5*j])){
        withstart = stoi(s2[3+6*i])-stoi(s2[2+6*i])+2;
        withlast = stoi(s2[3+6*i]);
        quastart = stoi(s1[2+5*j])+1;
        qualast = stoi(s1[2+5*j])+stoi(s1[3+5*j]);

        if(withstart<=qualast && qualast<=withlast && quastart<=withstart){
          s1[3+5*j]=to_string(qualast-withstart+stoi(s1[3+5*j]));
          break;
        }else if(withstart<=quastart && quastart<=withlast && withlast<=qualast){
          s1[2+5*j]=to_string(withstart-1);
          s1[3+5*j]=to_string(quastart-withstart+stoi(s1[3+5*j]));
          s1.insert(s1.begin()+insert2*5, s1[4+5*j]);
          s1.insert(s1.begin()+insert2*5, s1[4+5*j]);
          s1.insert(s1.begin()+insert2*5, s1[4+5*j]);
          s1.insert(s1.begin()+insert2*5, s1[4+5*j]);
          s1.insert(s1.begin()+insert2*5, s1[4+5*j]);
          s1.erase(s1.begin()+5*(j+1));
          s1.erase(s1.begin()+5*(j+1));
          s1.erase(s1.begin()+5*(j+1));
          s1.erase(s1.begin()+5*(j+1));
          s1.erase(s1.begin()+5*(j+1));
          s1.shrink_to_fit();
          break;
        }else if(withstart<=quastart && qualast<=withlast){
          s1[2+5*j]=to_string(withstart-1);
          s1[3+5*j]=to_string(withlast-withstart+1);
          s1.insert(s1.begin()+insert2*5, s1[4+5*j]);
          s1.insert(s1.begin()+insert2*5, s1[4+5*j]);
          s1.insert(s1.begin()+insert2*5, s1[4+5*j]);
          s1.insert(s1.begin()+insert2*5, s1[4+5*j]);
          s1.insert(s1.begin()+insert2*5, s1[4+5*j]);
          s1.erase(s1.begin()+5*(j+1));
          s1.erase(s1.begin()+5*(j+1));
          s1.erase(s1.begin()+5*(j+1));
          s1.erase(s1.begin()+5*(j+1));
          s1.erase(s1.begin()+5*(j+1));
          s1.shrink_to_fit();
          break;
        }else if(quastart<=withstart && withlast<=qualast){
          break;
        }
      }else if(stoi(s2[3+6*i])<stoi(s1[2+5*j])+1){
        s1.insert(s1.begin()+insert2*5, s2[4+6*i]);
        s1.insert(s1.begin()+insert2*5, to_string(stoi(s2[2+6*i])-1));
        s1.insert(s1.begin()+insert2*5, to_string(stoi(s2[3+6*i])-stoi(s2[2+6*i])+1));
        s1.insert(s1.begin()+insert2*5, s2[1+6*i]);
        s1.insert(s1.begin()+insert2*5, s2[0+6*i]);
        break;
      }else if(j==s1.size()/5-1){
        s1.insert(s1.begin()+insert2*5, s2[4+6*i]);
        s1.insert(s1.begin()+insert2*5, to_string(stoi(s2[2+6*i])-1));
        s1.insert(s1.begin()+insert2*5, to_string(stoi(s2[3+6*i])-stoi(s2[2+6*i])+1));
        s1.insert(s1.begin()+insert2*5, s2[1+6*i]);
        s1.insert(s1.begin()+insert2*5, s2[0+6*i]);
        break;
      }
    }
  }
  s2.clear();
  s2.shrink_to_fit(); //メモリ削減


  ifstream inputfile3(infile3);
  vector<string> s4; //6列 RfH
  while(getline(inputfile3, line)){
    vector<string> s3;
    s3 = tovector(line);
    s4.insert(s4.end(), s3.begin(), s3.end());
    s3.clear();
    s3.shrink_to_fit();
  }
  inputfile3.close();
  cout<<"with before RfH"<<endl;

  for (int i = 0; i < s4.size()/6; ++i){// RfH

    int insert2=s1.size()/5-1;
    int l=0;
    for (int j = 0; j < s1.size()/5; ++j){
      if(stoi(s1[2+5*j])==stoi(s4[2+6*i]) && l==0){
        insert2=j;
        l++;
      }
      if(stoi(s1[2+5*j])>stoi(s4[2+6*i]) && l==0){
        insert2=j;
        l++;
      }
      if(stoi(s4[0+6*i]) == stoi(s1[0+5*j])){
        int hosstart = stoi(s4[2+6*i])+1;
        int hoslast = stoi(s4[2+6*i])+stoi(s4[5+6*i]);
        int quastart = stoi(s1[2+5*j])+1;
        int qualast = stoi(s1[2+5*j])+stoi(s1[3+5*j]);

        if(hosstart<=qualast && qualast<=hoslast && quastart<=hosstart){
          s1[3+5*j]=to_string(qualast-hosstart+stoi(s1[3+5*j]));
          break;
        }else if(hosstart<=quastart && quastart<=hoslast && hoslast<=qualast){
          s1[2+5*j]=to_string(hosstart-1);
          s1[3+5*j]=to_string(quastart-hosstart+stoi(s1[3+5*j]));
          s1.insert(s1.begin()+insert2*5, s1[4+5*j]);
          s1.insert(s1.begin()+insert2*5, s1[4+5*j]);
          s1.insert(s1.begin()+insert2*5, s1[4+5*j]);
          s1.insert(s1.begin()+insert2*5, s1[4+5*j]);
          s1.insert(s1.begin()+insert2*5, s1[4+5*j]);
          s1.erase(s1.begin()+5*(j+1));
          s1.erase(s1.begin()+5*(j+1));
          s1.erase(s1.begin()+5*(j+1));
          s1.erase(s1.begin()+5*(j+1));
          s1.erase(s1.begin()+5*(j+1));
          break;
        }else if(hosstart<=quastart && qualast<=hoslast){
          s1[2+5*j]=to_string(hosstart-1);
          s1[3+5*j]=to_string(hoslast-hosstart+1);
          s1.insert(s1.begin()+insert2*5, s1[4+5*j]);
          s1.insert(s1.begin()+insert2*5, s1[4+5*j]);
          s1.insert(s1.begin()+insert2*5, s1[4+5*j]);
          s1.insert(s1.begin()+insert2*5, s1[4+5*j]);
          s1.insert(s1.begin()+insert2*5, s1[4+5*j]);
          s1.erase(s1.begin()+5*(j+1));
          s1.erase(s1.begin()+5*(j+1));
          s1.erase(s1.begin()+5*(j+1));
          s1.erase(s1.begin()+5*(j+1));
          s1.erase(s1.begin()+5*(j+1));
          break;
        }
      }else if(stoi(s4[2+6*i])+stoi(s4[5+6*i])<stoi(s1[2+5*j])+1){
        s1.insert(s1.begin()+insert2*5, s4[3+6*i]);
        s1.insert(s1.begin()+insert2*5, s4[5+6*i]);
        s1.insert(s1.begin()+insert2*5, s4[2+6*i]);
        s1.insert(s1.begin()+insert2*5, "0");
        s1.insert(s1.begin()+insert2*5, s4[0+6*i]);
        break;
      }else if(j==s1.size()/5-1){
        s1.insert(s1.begin()+insert2*5, s4[3+6*i]);
        s1.insert(s1.begin()+insert2*5, s4[5+6*i]);
        s1.insert(s1.begin()+insert2*5, s4[2+6*i]);
        s1.insert(s1.begin()+insert2*5, "0");
        s1.insert(s1.begin()+insert2*5, s4[0+6*i]);
        break;
      }
    }
  }

  s4.clear();
  s4.shrink_to_fit(); //メモリ削減

  ifstream inputfile4(infile4);
  vector<string> s5; //5列 Dead
  while(getline(inputfile4, line)){
    vector<string> s3;
    s3 = tovector(line);
    s5.insert(s5.end(), s3.begin(), s3.end());
    s3.clear();
    s3.shrink_to_fit();
  }
  inputfile4.close();
  cout<<"with before Dead"<<endl;

  for (int i = 0; i < s5.size()/5; ++i){// Dead
    if(stoi(s5[1+5*i]) == 1){
      for (int j = 0; j < s1.size()/5; ++j){
        if(stoi(s5[0+5*i]) == stoi(s1[0+5*j])){
          int deadstart = stoi(s5[2+5*i])+1;
          int quastart = stoi(s1[2+5*j])+1;
          int qualast = stoi(s1[2+5*j])+stoi(s1[3+5*j]);

          if(deadstart<=qualast){
            s1[3+5*j]=to_string(deadstart-quastart+1);
          }
        }
      }
    }
  }

  s5.clear();
  s5.shrink_to_fit(); //メモリ削減

  ofstream outputfile(outfile, ios::trunc);
  for (int i = 0; i < s1.size()/5; ++i){
    outputfile<<s1[0+i*5]<<" "<<s1[1+i*5]<<" "<<s1[2+i*5]<<" "<<s1[3+i*5]<<" "<<s1[4+i*5]<<endl;
  }
  outputfile.close();
}

void daycount(string outfile, int vaccine, string rn){
  ifstream inputfile1(rn + "Quarantined4.txt");
  ofstream outputfile(outfile);
  string line;
  int quacount = 0;
  while(getline(inputfile1, line)){
    vector<string> s2;
    s2 = tovector(line);
    if(s2[4]=="2" || s2[4]=="3"){
      quacount += stoi(s2[3]);
    }
  }
  outputfile << "休んだ日数の総計 " << quacount << endl;
  outputfile << "金額 " << quacount*145 << endl;

  inputfile1.close();

  ////////////////////

  ifstream inputfile2(rn + "RecoveredFromHospital.txt");
  int hoscount = 0;
  int hoscost[2][5] = {{10880, 15014, 17012, 20304, 11451}, {81596, 41918, 45722, 43309, 16750}};
  while(getline(inputfile2, line)){
    vector<string> s2;
    s2 = tovector(line);
    hoscount += hoscost[stoi(s2[4])][stoi(s2[3])];
  }
  outputfile << "入院費 " << hoscount << endl;
  inputfile2.close();
  
  //////////////////////////

  ifstream inputfile3(rn + "Recovered.txt");
  int outpatientcount = 0;
  int outpatientcost[2][5] = {{167, 95, 115, 130, 242}, {574, 649, 700, 730, 476}};
  while(getline(inputfile3, line)){
    vector<string> s2;
    s2 = tovector(line);
    if(s2[1] == "0"){
      outpatientcount += 3;
    }else{
      outpatientcount += outpatientcost[stoi(s2[5])][stoi(s2[4])];
    }
  }
  outputfile << "外来費 " << outpatientcount << endl;
  inputfile3.close();

  /////////////////////////

  ifstream inputfile4(rn + "Dead.txt");
  int deadcount = 0;
  int deadcost[5] = {1520471, 3077881, 6882779, 3698753, 1381123};
  while(getline(inputfile4, line)){
    vector<string> s2;
    s2 = tovector(line);
    deadcount += deadcost[stoi(s2[3])];
  }
  outputfile << "VSL " << deadcount << endl;
  inputfile4.close();

  /////////////////////////

  outputfile << "number of Vaccines " << vaccine << endl;
  outputfile << "Vaccines cost " << vaccine*30 << endl;
  outputfile << endl;

  outputfile << "総計 " << quacount*145 + hoscount + outpatientcount + deadcount + vaccine*30 << endl;

  ifstream inputfile("config-twodose");
  vector<string> s1;
  while(getline(inputfile, line)){
    s1 = tovector(line);
    if(s1[0]=="seed"){
      outputfile << "seed " << s1[1] << endl;
    }
    if(s1[0]=="R0"){
      outputfile << "R0 " << s1[1] << endl;
    }
    if(s1[0]=="schoolclosuredays"){
      outputfile << "schoolclosuredays " << s1[1] << endl;
    }
    if(s1[0]=="isolation"){
      outputfile << "isolation " << s1[1] << endl;
    }
    if(s1[0]=="quarantine"){
      outputfile << "quarantine " << s1[1] << endl;
    }
    if(s1[0]=="quarantinedays"){
      outputfile << "quarantinedays " << s1[1] << endl;
    }
    if(s1[0]=="schoolclosurestudents"){
      outputfile << "schoolclosurestudents " << s1[1] << endl;
    }
  }


  outputfile.close();
}

void configRename(string R0, string schoolclosuredays, string isolation, string quarantine, string quarantinedays, string schoolclosurestudents, string seed){
  ifstream inputfile("config-twodose");
  string line;
  vector<string> s1; //5列 Dead
  while(getline(inputfile, line)){
    vector<string> s2;
    s2 = tovector(line);
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
    if(s2[0]=="quarantinedays"){
      s2[1]=quarantinedays;
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

  string r0[2]={"1.5","1.7"};
  string schoolclosuredays[2]={"3","5"};
  string isolation[2]={"0.8","0.95"};
  string quarantine[2]={"0.3","0.5"};
  string quarantinedays[2]={"3","5"};
  string schoolclosurestudents[2]={"10","15"};
  string seed[10]={"1","2","3","4","5","6","7","8","9","10"};
  for (int a = 0; a <2; ++a){
    for (int b = 0; b < 2; ++b){
      for (int c = 0; c < 2; ++c){
        for (int d = 0; d < 2; ++d){
          for (int e = 0; e < 2; ++e){
            for (int f = 0; f < 2; ++f){
              for (int i = 0; i < 10; ++i){
                configRename(r0[a],schoolclosuredays[b],isolation[c],quarantine[d],quarantinedays[e],schoolclosurestudents[f],seed[i]);
                EpiModelParameters parms(configname);
                EpiModel model(parms);
                model.run();
                int vaccine_number = model.run();
                ifstream runnumber("run-number");
                string line;
                vector<string> s2;
                while(getline(runnumber, line)){
                  s2 = tovector(line);
                }
                runnumber.close();
                string rn = to_string(stoi(s2[0])-1);
                qua(rn + "Quarantined.txt", rn + "Quarantined2.txt");
                cout<<"qua ok"<<endl;
                withdraw(rn + "Quarantined2.txt", rn + "Recovered.txt", rn + "RecoveredFromHospital.txt", rn + "Dead.txt", rn + "Quarantined3.txt");
                cout<<"with ok"<<endl;
                qua(rn + "Quarantined3.txt", rn + "Quarantined4.txt");
                cout<<"qua ok"<<endl;
                daycount(rn + "AllResult.text", vaccine_number, rn);
              }
            }
          }
        }
      }
    }
  }
  return 0;
}

#endif

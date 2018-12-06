#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(){
	ifstream ifs("Quarantined.txt");
	
	string str;
	vector<string> v;

	while(getline(ifs, str)){
		if(v.size() == 0){
			v.push_back(str);
		}else{
			for(int i=0; i < v.size(); i++){
				if(v[i] != str){
					if(i == v.size()-1){
						v.push_back(str);
					}
					continue;
				}else{
					break;
				}
			}
		}
	}
	ofstream ofs("count.text");
	ofstream ofs2("quaratineds.text");
	ofstream ofs3("withdraws.text");

	ofs <<"Qua "<<v.size() << endl;

	for (int i = 0; i < v.size(); ++i)
	{
		ofs2 << v[i] << endl;
	}

	v.clear();

	////////////////////

	ifstream ifs2("Withdrawed.txt");

	while(getline(ifs2, str)){
		if(v.size() == 0){
			v.push_back(str);
		}else{
			for(int i=0; i < v.size(); i++){
				if(v[i] != str){
					if(i == v.size()-1){
						v.push_back(str);
					}
					continue;
				}else{
					break;
				}
			}
		}
	}

	ofs <<"Ind "<<v.size() << endl;

	for (int i = 0; i < v.size(); ++i)
	{
		ofs3 << v[i] << endl;
	}

	return 0;
}
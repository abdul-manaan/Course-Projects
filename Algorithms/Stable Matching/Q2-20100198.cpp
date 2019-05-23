/********************************************************************************************
 Name : Abdul Manan
 Roll# 2020-10-0198

 Algorithm:
 	Make two preferences List i.e. one for locations and other for teams
 	Run Gayle Shapely as folow: 
    While (somebody_is_not_matched and has not gone to everyone):
    	x = someone that is not matched
    	y = its next preference
    	if (y prefers x over current_preferece)
    		x is matched with y
    		y previous matched is set to unmatched
    	else:
    		continue

Space:
	O(n^2) to store two 2-d arrays preferences.i.e. list of locations and  teams

Complexity:
	O(n^2) worst case ... n is number of locations or teams

********************************************************************************************/

#include<iostream>
#include<fstream>
#include<vector>
#include<stdlib.h>
#include <utility>
using namespace std;


struct data
{
	vector< vector<int> > schedule;
	int n;
	int m;
	vector< vector<int> > teams;
	vector< vector<int> > locations;
};

vector<string> splitStr(string line);
int getNum(string s);
void readInput(data* Data, string fileName);
void PrintInput(data* Data);

bool isRemain(int* x, int n){
	for (int i = 0; i < n; ++i)
	{
		if (x[i] == 0)
		{
			return true;
		}
	}
	return false;
}
int somePlaneLeft(int* M, int* h, int n){
	for (int i = 0; i < n; ++i)
	{
		if (M[i] == 0)
		{
			if (h[i] < n)
			{
				return i;
			}
		}
	}
	return -1;
}

bool isPreference(data* Data,int prev,int cur,int team){
	for (int i = 0; i < Data->n; ++i)
	{
		if (Data->teams[team][i] == prev)
		{
			return false;
		} else if(Data->teams[team][i] == cur){
			return true;
		}
	}
}

bool findP(vector<pair<int,int>> v, pair<int,int> x){
	for (int i = 0; i < v.size(); ++i)
	{
		if (v[i].first == x.first && v[i].second == x.second)
		{
			return true;
		}
	}
	return false;
}

void algo(data* Data){
	int n = Data->n;
	int M[n];
	
	int proposed[n];
	int AP[n];

	for (int i = 0; i < n; ++i)
	{
		M[i] = 0;
		proposed[i] = 0;
		AP[i] = 0;
	}

	while(somePlaneLeft(M,proposed,n) != -1) {
		int h = somePlaneLeft(M,proposed,n);
		int s = Data->locations[h][proposed[h]];
		s = s-1;
		proposed[h]++;

		if (AP[s] == 0 )
		{
			M[h] = s+1;
			AP[s] = h+1;
		} else if(isPreference(Data,AP[s],h+1,s) ){
			M[AP[s]-1] = 0;
			M[h] = s+1;
			AP[s] = h+1;
		} else {
			M[h] = 0;
		}
	}
	for (int i = 0; i < n; ++i)
	{
		if (M[i] != 0)
		{
			cout << "L" << i+1 << " T" << M[i] << endl;
		}
	}
}
int main(){
	string fileName;
	cout << "Enter fileName: ";
	getline(cin,fileName);
	data* Data = new data;
	readInput( Data, fileName);
	//PrintInput(Data);
	algo(Data);
}











/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////						 ////////////////////////////////////////////////
//////////////////////////	Function Definitions ////////////////////////////////////////////////
//////////////////////////						 ////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////




vector<string> splitStr(string line){
        int n = line.length();
        int start = 0;
        vector<string> result;
        for (int i = 0; i < n; ++i) {
                if( !isalnum(line[i]) && line[i] != '-' ) {
                        if(i > start) {
                                result.push_back(line.substr(start, i-start));
                        }
                        start = i+1;

                }
        }
        if(start < n) {
                result.push_back(line.substr(start,n-start));
        }
        return result;
}


int getNum(string s){
   for (unsigned int i = 0; i < s.length(); ++i)
   {
      if (isdigit(s[i]))
      {
         return stoi(s.substr(i));
      }
   }
   cout << "Error in Node Num" << endl;
   return -1;
}


void readInput(data* Data, string fileName){
	ifstream file;
	file.open(fileName.c_str());
	if (!file) {
    	cout << "Unable to open file file.txt" << endl;
    	exit(1);   // call system to stop
	}

	string s;
	int n;
	int m;
	getline(file,s);
	n = stoi(splitStr(s)[1]);
	Data->n = n;


	for (int i = 0; i < n; ++i)
	{
		getline(file,s);
		vector<string> vec = splitStr(s);
		if (vec.size() < n+1)
		{
			cout << "Error in File" << endl;
			exit(1);
		}
		Data->m = vec.size()-1;
		m = Data->m;
		int m = getNum(vec[0]);
		vector<int> xx;
		vector<int> eachTeam;
		for (int i = 1; i < vec.size(); ++i)
		{
			if(vec[i] == "-"){
				xx.push_back(0);
			}
			else{
				xx.push_back( getNum(vec[i]) );
				eachTeam.push_back( getNum(vec[i]) );
			}
		}
		Data->schedule.push_back(xx);
		Data->teams.push_back(eachTeam);
	}
	Data->locations.resize(n);
	for (int i = m-1; i >= 0; --i)
	{
		for (int j =0; j < n; j++)
		{
			if(Data->schedule[j][i]){
				int n = Data->schedule[j][i];
				Data->locations[n-1].push_back(j+1);
			}
		}
	}

	return;
}

void PrintInput(data* Data){
	for (int i = 0; i < Data->n; ++i)
	{
		cout << "T" << i+1 << ": ";
		for (int j = 0; j < Data->m; ++j)
		{
			cout << Data->schedule[i][j] << " " ; 
		}
		cout << endl;
	}
	for (int i = 0; i < Data->n; ++i)
	{
		cout << "T" << i << " :" ;
		for (int j = 0; j < Data->n; ++j)
		{
			cout << Data->teams[i][j] << " ";
		}
		cout << endl;
	}
	for (int i = 0; i < Data->n; ++i)
	{
		cout << "L" << i << " :" ;
		for (int j = 0; j < Data->n; ++j)
		{
			cout << Data->locations[i][j] << " ";
		}
		cout << endl;
	}

}

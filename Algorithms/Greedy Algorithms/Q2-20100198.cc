/*******************************************************************************************************
*Name : Abdul Manan
*Roll# 2020-10-0198
*
*Algorithm:
* First of all , check if you can go to next station with current petrol
* 	if you can go, just go
*   if you can't, find how much petrol required to reach next station, fill that petrol and go
*  
* 
*Time Complexity:
*  O(n)
*Space Complexity:
* O(n) where n is number of stations
********************************************************************************************************/



#include <bits/stdc++.h>


using namespace std;


struct given{
	double TotalCap;
	double LtrPerKm;
	double LtrPerMin;
	double *station;
	int n;
};

void ReadInput(given* userInput, string fileName){
	ifstream file;
	file.open(fileName.c_str());
	if (!file) {
    	cout << "Unable to open file file.txt" << endl;
    	exit(1);   // call system to stop
	}

	string s;
	int n;
	file >> s >> userInput->TotalCap;
	file >> s >> userInput->LtrPerKm;
	file >> s >> userInput->LtrPerMin;
	file >> s >> userInput->n;
	n = userInput->n;
	userInput->station = new double[n+1];
	for (int i = 0; i <= n; ++i)
	{
		file >> s >> userInput->station[i];
	}
	return;
}

void PrintInput(given* userInput){
	cout << userInput->TotalCap << endl;
	cout << userInput->LtrPerKm << endl;
	cout << userInput->LtrPerMin << endl;
	for (int i = 0; i <= userInput->n; ++i)
	{
		cout << userInput->station[i] << endl;
	}
}

void GreedyAlgo(given* userInput){

	double currCap = userInput->TotalCap;
	//double T = userInput->TotalCap;
	double R = userInput->LtrPerKm;
	double F = userInput->LtrPerMin;

	//double start;// = 0;
	currCap = currCap - userInput->station[0]*R;
	double TotalWaitTime = 0;
	for (int i = 0; i < userInput->n; ++i)
	{
		double distance = userInput->station[i+1] - userInput->station[i];
		if (distance > currCap/R)
		{
			cout << "Station: "  <<  i+1 
				 << " Petrol (lites): "  <<  (distance - currCap/R)*R 
				 << " Wait (minutes): "    <<  (distance - currCap/R)*R/F << endl;
			TotalWaitTime += (distance - currCap/R)*R/F;
			currCap = currCap + (distance - currCap/R)*R; 
		}

		currCap = currCap - (distance)*R;
		//cout << "Curr:" << currCap << endl;
		//start = userInput->station[i];
	}

	cout << "Total Waiting Time (minutes): " << TotalWaitTime << endl;
	return;
}
int main(){
	given* userInput = new given;
	string fileName;
	cout << "Enter File Name: ";
	cin >> fileName;
	ReadInput(userInput,fileName);
	//PrintInput(userInput);
	GreedyAlgo(userInput);
}
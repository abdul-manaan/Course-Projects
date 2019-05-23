/******************************************************************************************
*Name: Abdul Manan
*Roll# 2020-10-0198
*
*Recurrence Realtion:

	func(n):
		if n < 1 : 
			return 0
		else  
			return max(Rh[n] + fn(n-2) , Rl[n] + fn(n-1))
	Here n is week number.

*Complexity:
	Without memoization: O(2^N-1)
	With memoization:    O(n) where n is number of weeks
	Actually, there are n subproblems and each subproblem is computed only once in constant time.
********************************************************************************************/

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;


class Part1{
	unsigned numOfWeeks;
	int* RevenueHigh;
	int* RevenueLow;
	int *memo;
	vector<string> result;
public:
	Part1(string fileName);
	int func(int n);
	void printResult();
	void findPath();
	unsigned getNumofWeeks();
};


int main(){
	string fileName;
	cout << "Enter fileName: ";
	getline(cin,fileName);
	Part1 part(fileName);
	part.func(part.getNumofWeeks());
	part.printResult();
}

Part1::Part1(string fileName){
	ifstream file;
	file.open(fileName.c_str());
	if (!file) {
    	cout << "Unable to open file file.txt" << endl;
    	exit(1);   // call system to stop
	}
	string ch;
	file >> ch >> numOfWeeks;
	file >> ch;

	RevenueHigh = new int[numOfWeeks+1];
	RevenueLow = new int[numOfWeeks+1];
	memo = new int[numOfWeeks+1];
	for (unsigned i = 1; i <= numOfWeeks; ++i)
	{
		file >> RevenueHigh[i];
	}
	file >> ch;
	for (unsigned i = 1; i <= numOfWeeks; ++i)
	{
		file >> RevenueLow[i];
	}

	for (unsigned i = 0; i <= numOfWeeks; ++i)
	{
		memo[i] = -1;
	}
	memo[0] = 0;
	memo[1] = max(RevenueLow[1], RevenueHigh[1]);
}
int Part1::func(int n){
	if (memo[n] < 0){
		int isHigh = RevenueHigh[n] + func(n-2);
		int isLow  = func(n-1) + RevenueLow[n];
		memo[n] = max(isHigh, isLow);
	}
	return memo[n];
}

void Part1::printResult(){
	findPath();
	for (int i = numOfWeeks-1; i >=0; --i)
	{
		cout << result[i] << endl;
	}
	cout << "Total Revenue: " << memo[numOfWeeks] << endl;
}

void Part1::findPath(){
	int n = numOfWeeks;
	while(n > 0){
		if (memo[n] - memo[n-2] == RevenueHigh[n])
		{
			result.push_back("Week# " + to_string(n) + " High " + to_string(RevenueHigh[n]));
			result.push_back("Week# " + to_string(n) + " Priming " );
			n -= 1;
		} else {
			result.push_back("Week# " + to_string(n) + " Low " + to_string(RevenueLow[n]));
		}
		n--;
	}
}

unsigned Part1::getNumofWeeks(){
	return numOfWeeks;
}
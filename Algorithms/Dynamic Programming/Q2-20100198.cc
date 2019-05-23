/******************************************************************************************
*Name: Abdul Manan
*Roll# 2020-10-0198
*
*Recurrence Realtion:

	func(start,end):
		if start +1 >= end:
			return 0
		else
			return (Value[end] - Value[start]) +  min( func(start,k), func(k,end)) 		
	Here k < end and k > start.
	Value[i] represents marking at i.

	Here start and end represents a scale that starts at mark Value[start] and ends at Value[end].
	and func(start,end) gives minimum cost to cut a ruler that start and ends at Value[start] 
	and Value[end] respectively.

*Complexity:
Without memoization: O(N^N) --- because there are N^N subproblems (including overlappping)
with memoization   : O(N^2) --- where N is the number of cuts.
********************************************************************************************/

#include <iostream>
#include <fstream>
#include <vector>
#include <limits.h>
using namespace std;


class Part2{
	unsigned length;
	vector<unsigned> markings;
	int **isSelected;
	int **memo;
public:
	Part2(string fileName);
	int func(int,int);
	unsigned getTotalMarks();
	void printOrdering(int,int);
};


int main(){
	string fileName;
	cout << "Enter fileName: ";
	getline(cin,fileName);
	Part2 part(fileName);
	int result =  part.func(0,part.getTotalMarks()-1);
	cout << "Optimal Cut Ordering: ";
	part.printOrdering(0,part.getTotalMarks()-1);
	cout << endl;
	cout << "Least Cost: " << result << endl;
}	

Part2::Part2(string fileName){
	ifstream file;
	file.open(fileName.c_str());
	if (!file) {
    	cout << "Unable to open file file.txt" << endl;
    	exit(1);   // call system to stop
	}
	string ch;
	file >> ch >> length;
	file >> ch;
	markings.push_back(0);
	int mark;
	while(file >> mark)
	{
		markings.push_back(mark);
	}
	markings.push_back(length);

	isSelected = new int*[markings.size() +1];
	memo = new int*[markings.size() +1];
	for (int i = 0; i < markings.size()+1; ++i)
	{
		isSelected[i] = new int[markings.size() +1];
		memo[i] = new int[markings.size() +1];
	}

	for (int i = 0; i < markings.size(); ++i)
	{
		for (int j = 0; j < markings.size(); ++j)
		{
			memo[i][j] = 0;
		}
	}
}

int Part2::func(int begin, int end){
	if (end <= begin + 1)
	{
		return 0;
	} else {
		if (memo[begin][end])
		{
			return memo[begin][end];
		}
		int minim = INT_MAX;
		for (int k = begin+1; k < end; ++k)
		{
			int newMin = min(minim, func(begin,k) + func(k,end));
			if (minim != newMin)
			{
				isSelected[begin][end] = k;
				minim = newMin;
			}
		}
		memo[begin][end] = (markings[end] - markings[begin]) + minim;
		return memo[begin][end];
	} 
}


void Part2::printOrdering(int start, int end){
	if (end <= start+1)
	{
		return;
	}
	int k = isSelected[start][end];
	cout << markings[k] << " ";
	printOrdering(start,k);
	printOrdering(k,end);
}


unsigned Part2::getTotalMarks(){
	return markings.size();
}
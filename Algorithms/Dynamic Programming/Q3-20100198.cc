/******************************************************************************************
*Name: Abdul Manan
*Roll# 2020-10-0198
*
*Recurrence Realtion:

	func(size,sum):
		if size < 1:
			return (sum == 0) ? true : false;
		else:
			return (size-1 , sum - Value[size-1]) || (size-1 , sum + Value[size-1])
		
		Here size is size of array that has to divide and sum represensts differen of sums of two sets.
		Note Either value[size-1] can be in Set 1 or Set 2. 

*Complexity:
	without memoization : O(n^n)
	With memoization:     O(n.log n) n is the size of array 
	There are n subproblems and each subproblems takes O(log n) time
********************************************************************************************/

#include <iostream>
#include <fstream>
#include <vector>
#include <limits.h>
#include <map>
using namespace std;

struct log{
	vector<int> set1;
	vector<int> set2;
};

class Part3{
	unsigned sum;
	vector<int> list;
	map< pair<int,int>, pair<bool,log*> > memo;
public:
	Part3(string fileName);
	pair<bool,log*> func(int,int);
	unsigned getTotalMarks();
	void printOrdering(int,int);
	int listSize();
	int getSum();
};


int main(){
	string fileName;
	cout << "Enter fileName: ";
	getline(cin,fileName);
	Part3 part(fileName);
	auto result =  part.func(part.listSize(), part.getSum());
	// cout << result << endl;

	if (result.first)
	{
		cout << "POSSIBLE" << endl;
		cout << "Set1 : ";
		int sum1 = 0;
		for (int i = 0; i < result.second->set1.size(); ++i)
		{
			sum1 += result.second->set1[i];
			cout << result.second->set1[i] << " ";
		}
		cout << "Sum " << sum1 << endl << "Set2: ";
		int sum2 = 0;

		for (int i = 0; i < result.second->set2.size(); ++i)
		{
			sum2 += result.second->set2[i];
			cout << result.second->set2[i] << " ";
		}
		cout << "Sum " << sum2 << endl;

		cout <<  "Difference " << sum1 << " - " << sum2 << " = " << sum1-sum2 << endl;
	} else {
		cout << "IMPOSSIBLE" << endl;
	}
}	

Part3::Part3(string fileName){
	ifstream file;
	file.open(fileName.c_str());
	if (!file) {
    	cout << "Unable to open file file.txt" << endl;
    	exit(1);   // call system to stop
	}
	string ch;
	file >> ch >> sum;
	file >> ch;
	int eachNum;
	while(file >> eachNum)
	{
		list.push_back(eachNum);
	}
}

pair<bool,log*> Part3::func(int size, int sum){
	log *myLog = new log;
	if (size == 0)
	{
		return (sum == 0) ? make_pair(true,myLog) : make_pair(false,myLog);
	} else {
		pair<bool,log*> xLog = func(size-1,sum-list[size-1]);
		pair<bool,log*> yLog = func(size-1,sum+list[size-1]);
		if (xLog.first)
		{
			xLog.second->set1.push_back(list[size-1]);
			memo[make_pair(size,sum)] = xLog;
			return xLog;
		} else if (yLog.first)
		{
			yLog.second->set2.push_back(list[size-1]);
			memo[make_pair(size,sum)] = yLog;
			return yLog;
		}else {
			memo[make_pair(size,sum)] = make_pair(false,myLog);
			return make_pair(false,myLog);
		}
	}
}

int Part3::listSize(){
	return list.size();
}

int Part3::getSum(){
	return sum;
}
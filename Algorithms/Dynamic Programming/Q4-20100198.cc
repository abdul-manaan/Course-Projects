/******************************************************************************************
*Name: Abdul Manan
*Roll# 2020-10-0198
*
*Recurrence Realtion:

	func(string1,string2,string3):
		if string3.size < 1:
			return true;
		result1 =  (string1[0] == string3[0]) ? func(string1[1..],string2,string3[1..])
		result2 =  (string2[0] == string3[0]) ? func(string1, string2[1...],string3[1..])
		return result2 || result1
		
		Here string3 is interleaving of string1 and string2 

*Complexity:
	Without memoization : O(2^ max(A,B))
	With memoization: O(A*B) A is size of string1 and B is size of string2.
********************************************************************************************/

#include <iostream>
#include <fstream>
#include <vector>
#include <limits.h>
#include <map>
#include <string>
using namespace std;


class Part4{
	string str1;
	string str2;
	string str3;
	map<pair<int,int>, bool> memo;
public:
	vector<int> setNum;
	Part4(string fileName);
	bool func(int,int,int);
	void printPath();
	int s1L(){return str1.length();}
	int s2L(){return str2.length();}
	int s3L(){return str3.length();}
};


int main(){
	string fileName = "Q4-input-A.txt";
	cout << "Enter fileName: ";
	getline(cin,fileName);
	Part4 part(fileName);
	int result =  part.func(part.s1L(),part.s2L(),part.s3L() );
	if (result)
	{
		cout << "VALID" << endl;
		part.printPath();
	} else {
		cout << "INVALID" << endl;
	}

	// delete []part->setNum;
	// delete part;
}	

Part4::Part4(string fileName){
	ifstream file;
	file.open(fileName.c_str());
	if (!file) {
    	cout << "Unable to open file file.txt" << endl;
    	return;
	}
	string ch;
	getline(file,str1);
	getline(file,str2);
	getline(file,str3);
	setNum.resize(str3.length()+1);
}

bool Part4::func(int s1Len, int s2Len, int s3Len){
	if (memo.find(make_pair(s1Len,s2Len)) != memo.end())
	{
		return memo[make_pair(s1Len,s2Len)];
	}
	if (s3Len == 0)
	{
		return true;
	} else if (s1Len == 0)
	{
		bool res =  (str2.substr(0,s2Len) == str3.substr(0,s3Len)) ? true : false;
		for (int i = 0; i < s3Len; ++i)
		{
			setNum[i] = res ? 2 : 0;
		}
		return res;
	} else if(s2Len == 0)
	{
		bool res = (str1.substr(0,s1Len) == str3.substr(0,s3Len)) ? true : false;
		for (int i = 0; i < s3Len; ++i)
		{
			setNum[i] = res ? 1 : 0;
		}
		return res;
	} else
	{
		bool res1 = false;
		bool res2 = false;

		if (str1[s1Len-1] == str3[s3Len-1])
		{
			res1 =  func(s1Len-1, s2Len, s3Len-1);
		}
		
		if (str2[s2Len-1] == str3[s3Len-1] )
		{
			res2 = func(s1Len,s2Len-1, s3Len-1);
		} 

		if (res1)
		{
			setNum[s3Len-1] = res1;
		}
		if (res2) {
			setNum[s3Len-1] =  2;
		}
		
		memo[make_pair(s1Len,s2Len)] = setNum[s3Len-1];

		return (res1 || res2);
	}
}


void Part4::printPath(){
	for (unsigned i = 0; i < str3.length();)
	{
		int setNo = setNum[i];
		unsigned j = i;
		while(j < str3.length() && setNum[j] == setNo){
			j++;
		}
		cout << "S" << setNo << " " << str3.substr(i,j-i) << endl;;
		i += j-i;
	} 
}
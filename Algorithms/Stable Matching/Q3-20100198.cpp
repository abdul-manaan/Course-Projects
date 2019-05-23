/********************************************************************************************
 Name : Abdul Manan
 Roll# 2020-10-0198

 Algorithm:
	Make a 2d array of Board of n*n
	func(Board):
		divide n to 4 parts and recursively call func() on each
		if(n == 2):
			Put a char in three spaces available as one is fixed
	Note:
		While dividing, I fix a point in each part.  

Recurrence:
	T(n) = 4T(n/2) + c
Space:
	O(n^2) to store board

Time Complexity:
	O(n^2) n is number of rows/columns 

********************************************************************************************/



#include<iostream>
#include<fstream>
#include<vector>
#include<stdlib.h>
#include <utility>
using namespace std;


struct data
{
	char** board;
	int x,y;
	int n;
};

vector<string> splitStr(string line);
int getNum(string s);
void readInput(data* Data,string fileName);
void PrintInput(data* Data);

char x = 'A';
void algo(data* Data, pair<int,int> start,pair<int,int> end, pair<int,int> fixed){
	if ((end.first-start.first) < 1)
	{
		return;
	}
	if ((end.first - start.first ) == 1)
	{
		for (int i = start.first; i <= end.first; ++i)
		{
			for (int j = start.second; j <= end.second; ++j){
				if (i == fixed.first and j == fixed.second)
				{
					continue;
				} else {
					Data->board[i][j] = x;
				}
			}
		}
		x+=1; 
	} else{
		int mid1 = (end.first -start.first)/2 + start.first;
		int mid2 = (end.second -start.second)/2 + start.second;

		pair<int,int> start1 = start;
		pair<int,int> end1   = make_pair(mid1,mid2);
		pair<int,int> start2 = make_pair(mid1+1,mid2+1);
		pair<int,int> end2   = end;
		pair<int,int> start3 = make_pair(start.first,mid2+1);
		pair<int,int> end3   = make_pair(mid1,end.second);
		pair<int,int> start4 = make_pair(mid1+1,start.second);
		pair<int,int> end4   = make_pair(end.first,mid2);

		int n;
		if (fixed.first > mid1)
		{
			n = (fixed.second > mid2) ? 2 : 4;
		} else{
			n = (fixed.second > mid2) ? 3 : 1;
		}
		pair<int,int> fixed1 = end1; 
		pair<int,int> fixed2 = start2; 
		pair<int,int> fixed3 = make_pair(mid1,mid2+1);
		pair<int,int> fixed4 = make_pair(mid1+1,mid2);
		
		if (n == 1)
		{
			fixed1 = fixed;
			Data->board[mid1+1][mid2+1] = x;
			Data->board[mid1][mid2+1] = x;
			Data->board[mid1+1][mid2] = x++;
		} else if (n == 2){
			fixed2 = fixed;
			Data->board[mid1][mid2] = x;
			Data->board[mid1][mid2+1] = x;
			Data->board[mid1+1][mid2] = x++;
		} else if(n == 3){
			fixed3 = fixed;
			Data->board[mid1][mid2] = x;
			Data->board[mid1+1][mid2+1] = x;
			Data->board[mid1+1][mid2] = x++;
		} else{
			fixed4 = fixed;
			Data->board[mid1][mid2] = x;
			Data->board[mid1+1][mid2+1] = x;
			Data->board[mid1][mid2+1] = x++;
		}

		algo(Data,start1,end1,fixed1);
		algo(Data,start2,end2,fixed2);
		algo(Data,start3,end3,fixed3);
		algo(Data,start4,end4,fixed4);
	}
}


int main(){
	string fileName;
	cout << "Enter fileName: ";
	getline(cin,fileName);
	data* Data = new data;
	readInput( Data, fileName);
	algo(Data,make_pair(0,0), make_pair(Data->n-1,Data->n-1), make_pair(Data->x,Data->y));
	PrintInput(Data);
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

	char x;
	file >> x >> Data->n;
	file >> x >> Data->x;
	file >> x >> Data->y;

	Data->board = new char*[Data->n];
	for (int i = 0; i < Data->n; ++i)
	{
		Data->board[i] = new char[Data->n];
	}
	Data->board[Data->x][Data->y] = 'X';
	return;
}

void PrintInput(data* Data){
	for (int i = 0; i < Data->n; ++i)
	{
		for (int j = 0; j < Data->n; ++j)
		{
			cout << Data->board[i][j] << " ";
		}
		cout << endl;
	}
}

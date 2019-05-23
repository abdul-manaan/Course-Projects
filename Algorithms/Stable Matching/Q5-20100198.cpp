/********************************************************************************************
 Name : Abdul Manan
 Roll# 2020-10-0198

 Algorithm:
	Make a 2d array of Board of n*n
	func(value, n):
		if(n ==1):
			return value[1]
		lans = func(half1)
		rans = func(half2)
		if lans == rans:
			return lans
		else if no lans:
			if rans is max in value:
				return rans 
 			else:
 				return no Match


 Recurrence:
	T(n) = 2T(n/2) + c.n
 Space:
	O(n) i.e. n is size of array

 Complexity:
	O(n.log n) n is size of array

********************************************************************************************/


#include <iostream>
#include <vector>
#include <cstdlib>
#include <time.h>
using namespace std;


bool decode(int a, int b){
	return (a==b) ? true: false;
}
int counter(int* arr, int n, int start, int a){
	int c = 0;
	for (int i = start; i < start+n; ++i)
	{
		c += (decode(arr[i],a))? 1 : 0;
	}
	return c;
}

int algo(int* arr, int n, int start ){
	if (n == 1)
	{
		return arr[start];
	}
	int k = n/2;
	int left = algo(arr,k,start );
	int right = algo(arr, n-k, start+k);

	if (left == -1 && right == -1) {
		return -1;
	} else if (left == -1)	{
		int count = counter(arr,n,start,right);
		return (count > k) ? right : -1;
	} else if (right == -1)	{
		int count = counter(arr,n,start,left);
		return (count > k) ? left : -1;	
	} else if (decode(left,right)) {
		return left;
	} else{
		int lcount = counter(arr,n,start,left);
		int rcount = counter(arr,n,start,right);
		if (lcount > k)
		{
			return left;
		} else if (rcount > k)
		{
			return right;
		}
		return -1;
	}
}

int answer(int* arr, int n){
	int x = algo(arr,n,0);

	if (x == -1)
	{
		cout << "Failure" << endl;
	} else {
		cout << "Success" << endl;
		for (int i = 0; i < n; ++i)
		{
			if (decode(arr[i],x))
			{
				cout << i << " ";
			}
		}
		cout << endl;
	}
}
int main(){
	int n,m;
	cout << "Enter n: ";
	cin >> n;
	cout << "Enter m: ";
	cin >> m;
	int h[n];

	srand(time(0));
	for (int i = 0; i < n; ++i)
	{
		h[i] = rand()%m +1;
		cout << h[i] << " ";
	}
	cout << endl;
	answer(h, n);
}
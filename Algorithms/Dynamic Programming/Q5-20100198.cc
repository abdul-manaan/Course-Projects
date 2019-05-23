/******************************************************************************************
*Name: Abdul Manan
*Roll# 2020-10-0198
*
*
*Complexity:
	O(size^3)
	Becaue we have to traverse who 2d array to make a prefixSum MAtrix and then another traversal
	gives the result.
********************************************************************************************/

#include <iostream>
#include <limits.h>
#include <cstdlib>
#include <time.h>

using namespace std;


class Part5
{
	int size;
	int** field;
	int** prefixSum;
public:
	Part5(int n, int* arr);
	~Part5(){};
	void calPrefixSum();
	void func();
};

int main(){
	// srand(time(0));
	// int n;
	// cout << "Enter value of n: ";
	// cin >> n;
	// int arr[n*n];
	// for (int i = 0; i < n*n; ++i)
	// {
	// 	arr[i] = (rand()%1000) - rand()%400;
	// }
	// int n =5;
	// int arr[25] = { 1, 8, 33, -1, 20, -10, 2, 23, -7, 27, -27, 8, 6, 7, 32, -25, 4, -67, 4, 4, 3, 11, 5, 4, 5 };
	
	// int n = 8;
	// int arr[64] = { 25, 31, 1, 8, 33, -1, 20, -25, 14, 13, -10, 2, 23, -7, 27, 21, 31, 45, 11, -27, 8, 6, 7, 32, 12, -19, -25, 4, -67, 4, 4, 13, 14, -12, 10, 3, 11, 5, 4, 5, 4, 4, -25, 4, -67, 13, 12, -19, 22, -29, -15, 14, -37, -141, 14, 13, 11, -29, -15, 14, 17, 6, 4, 13 };
	
	int n = 8;
	int arr[64] = { 13, -3, 11, 18, -41, -75, -20, 25,11, -29, -15, 14, 17, 6, 4, 13,-93, -31, -10, 2, 23, -7, 27, 21, 2, -9, -25, 41, 67, 44, 14, -13, 14, -12, 10, 31, 13, 51, 14, 5, 4, 4, -25, 4, 67, 13, 21, -19,22, -77, -15, 53, 37, 47, 41, 13,-76, 45, 11, -27, 8, 6, 7, 32};
	Part5 part(n,arr);
	part.calPrefixSum();
	part.func();
}



Part5::Part5(int n, int* arr){
	size = n;
	field = new int*[n];
	for (int i = 0; i < n; ++i)
	{
		field[i] = new int[n];
	}


	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			field[i][j] = arr[i*n+j];
			cout << field[i][j] << " ";
		}
		cout << endl;
	}
	prefixSum = new int*[n];
	for (int i = 0; i < n; ++i)
	{
		prefixSum[i] = new int[n];
	}
}

void Part5::calPrefixSum(){
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			if (i > 0)
			{
				if (j > 0)
				{
					prefixSum[i][j] = field[i][j] + prefixSum[i-1][j] + prefixSum[i][j-1] - prefixSum[i-1][j-1];
				} else {
					prefixSum[i][j] = field[i][j] + prefixSum[i-1][j];
				}
			} else {
				if (j > 0)
				{
					prefixSum[i][j] = field[i][j] + prefixSum[i][j-1] ;
				}else {
					prefixSum[i][j] = field[i][j];
				}
			}
		}
	}

}

void Part5::func(){
	
	int width = 0;
	int bottomX = 0;
	int bottomY = 0;
	int topX = 0;
	int topY = 0;
	int sum = INT_MIN;

	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			int t_topX = i;
			int t_topY = j;
			int t_bottomY;
			int t_bottomX;
			int t_sum = INT_MIN;//prefixSum[i][j];
			for (int k = i, l = j; k < size && l < size; ++k)
			{
				if (i > 0)
				{
					if ( j > 0)
					{
						if (prefixSum[k][l] - prefixSum[i-1][l] - prefixSum[k][j-1] + prefixSum[i-1][j-1] > t_sum)
						{
							t_sum = prefixSum[k][l] - prefixSum[i-1][l] - prefixSum[k][j-1] + prefixSum[i-1][j-1];
							t_bottomX = k;
							t_bottomY = l;
						}
					} else {
						if (prefixSum[k][l] - prefixSum[i-1][l] > t_sum)
						{
							t_sum = prefixSum[k][l] - prefixSum[i-1][l];
							t_bottomX = k;
							t_bottomY = l;
						}						
					}
				} else {
					if (j > 0)
					{
						if (prefixSum[k][l] - prefixSum[k][j-1] > t_sum)
						{
							t_sum = prefixSum[k][l] - prefixSum[k][j-1];
							t_bottomX = k;
							t_bottomY = l;
						}
					} else {
						if (prefixSum[k][l] > t_sum)
						{
							t_sum = prefixSum[k][l];
							t_bottomX = k;
							t_bottomY = l;
						}
					}
				}
				l++;
			}

			if (t_sum > sum)
			{
				sum = t_sum;
				bottomY = t_bottomY;
				bottomX = t_bottomX;
				topX = t_topX;
				topY = t_topY;
			}
		}
	}



	cout << "Sum: " << sum << endl;
	cout << "Top: (" << topX << " , " << topY << ")" << endl;
	cout << "Bottom: (" << bottomX << " , " << bottomY << ")" << endl;
	cout << "Width: " << bottomX - topX + 1 << endl;

}
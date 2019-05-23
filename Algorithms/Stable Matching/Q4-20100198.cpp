/********************************************************************************************
 Name : Abdul Manan
 Roll# 2020-10-0198

 Algorithm:
	Make a 2d array of Board of n*n
	func(tree):
		if(!lans and !rans):
			return value[lans]
		lans = func(tree->left)
		rans = func(tree->right)
		calculate maxAncestory and return ans
 

 Recurrence:
	T(n) = 2T(n/2) + c
 Space:
	O(n) i.e. n is number of leaves nodes. It is actually (2n-1) ~ n

 Complexity:
	O(n) n is number of leave nodes

********************************************************************************************/



#include <iostream>
#include <bits/stdc++.h>
using namespace std;


struct node{
	int val;
	node* left;
	node* right;
	node(int x){
		val = x;
		left= right = NULL;
	}
};
map <int, int> leafN;  
struct maxS{
	int arbSum;
	int sum;
	int leaf1;
	int leaf2;
	vector<int> ancesX1;
	vector<int> ancesX2;
};

maxS* maxSum(node* root){
	if(!root->left and !root->right){

		maxS* x = new maxS;
		x->sum    = root->val;
		x->arbSum = root->val;
		x->leaf1  = root->val;
		x->leaf2  = -1;  
		x->ancesX1.push_back(root->val);
		
		return x;
	} else	{
		maxS* leftSum = maxSum(root->left);
		maxS* rightSum = maxSum(root->right);

		maxS* x = new maxS;
		x->leaf1 = (leftSum->leaf1 > leftSum->leaf2)? leftSum->leaf1 : leftSum->leaf2;
		x->leaf2 = (rightSum->leaf1 > rightSum->leaf2)? rightSum->leaf1 : rightSum->leaf2;

		x->sum = leftSum->arbSum + rightSum->arbSum + root->val;
		bool l = true, r = true;
		x->ancesX1.push_back(root->val);
		x->ancesX2.push_back(root->val);
		if (x->sum < leftSum->sum + root->val)
		{
			x->sum = leftSum->sum + root->val;
			x->leaf1 = leftSum->leaf1;
			x->leaf2 = leftSum->leaf2;

			x->ancesX1.insert(x->ancesX1.end(), leftSum->ancesX1.begin(), leftSum->ancesX1.end());
			x->ancesX2.insert(x->ancesX2.end(), leftSum->ancesX2.begin(), leftSum->ancesX2.end());
			l = false;
		}
		if(x->sum < rightSum->sum + root->val){
			x->sum   = rightSum->sum + root->val;
			x->leaf1 = rightSum->leaf1;
			x->leaf2 = rightSum->leaf2;

			x->ancesX1.insert(x->ancesX1.end(), rightSum->ancesX1.begin(), rightSum->ancesX1.end());
			x->ancesX2.insert(x->ancesX2.end(), rightSum->ancesX2.begin(), rightSum->ancesX2.end());
			r = false;
		}
		if (l and r)
		{
			auto lm = (leftSum->leaf1 > leftSum->leaf2)? leftSum->ancesX1 : leftSum->ancesX2;
			auto rm = (rightSum->leaf1 > rightSum->leaf2)? rightSum->ancesX1 : rightSum->ancesX2;
			x->ancesX1.insert(x->ancesX1.end(), lm.begin(), lm.end());
			x->ancesX2.insert(x->ancesX2.end(), rm.begin(), rm.end());
		}
		x->arbSum = (leftSum->arbSum > rightSum->arbSum) ? 
									leftSum->arbSum + root->val: 
									rightSum->arbSum + root->val; 

		return x;
	}
}

node* makeTreeFromArray(int* arr, int size, int n = 0){
	node* root =  new node(arr[n]);
	node* temp = root;


	int lh = (n << 1) +1; 
	int rh = (n << 1) +2;

	if (lh >= size)
	{
		temp->left  = NULL;
		temp->right = NULL;
		leafN.insert(make_pair(arr[n], n-size/2+1));
		return root;
	}
	


	temp->left = makeTreeFromArray(arr,size,lh);
	temp->right = makeTreeFromArray(arr,size,rh);

	return temp;
}

node* makeRandomTree(int n, int random = 100){
	int arr[n];
	for (int i = 0; i < n; ++i)
	{
		arr[i] = rand()%random;
	}
	return makeTreeFromArray(arr,n);
}
void printArray(vector<int> a){
	cout << "{ ";
	for (int i = 0; i < a.size(); ++i)
	{	
		cout << a[i];
		if (i != a.size()-1)
		{
			cout << " , ";
		}
	}
	cout << " } " << endl;

}
int main(){
	srand(time(0));
	int arr[15] = {17, 20, 4, 5, 7, 35, 6, 3, 6, 16, 12, 18, 1, 2, 3};
	//int arr[15] = {29, 6, 11, 15, 5, 10, 20, 1, 3, 4, 2, 15, 45, 10, 50};
	node* root = makeTreeFromArray(arr,15);

	maxS* ans = maxSum(root);
	cout << "xi = " << leafN[ans->leaf1] << endl; 
	cout << "xj = " << leafN[ans->leaf2] << endl; 
	cout <<  "Value of nodes in Ancestry (x" << leafN[ans->leaf1] << ") = "; printArray(ans->ancesX1);
	cout <<  "Value of nodes in Ancestry (x" << leafN[ans->leaf2] << ") = "; printArray(ans->ancesX2);
	cout <<  "Value of Max Ancestry (xi, xj) = " << ans->sum << endl;
}
/*******************************************************************************************************
*Name : Abdul Manan
*Roll# 2020-10-0198
*
*Algorithm:
* First of all , create a heap of all column enteries.
*   Take out a column with max avaialbe space and put piece in there
*   Repeat above step at each row until all pieces are placed
*  
* 
*Time Complexity:
*  O(n.log.n)
*Space Complexity:
* O(n^2) where n is number of rows
********************************************************************************************************/


#include <bits/stdc++.h>
#include "heap.cpp"
using namespace std;

struct columnEntry{
   int n;
   int val;
};

struct given{
   int n;
   int *row;
   int **matrix;
   MinHeap<columnEntry*> *column;
};

bool isGreater(columnEntry* a,columnEntry* b){
   return (a->val > b->val);
}

void ReadInput(given* userInput, string fileName){
   ifstream file;
   file.open(fileName.c_str());
   if (!file) {
      cout << "Unable to open file file.txt" << endl;
      return;   // call system to stop
   }

   string s;
   int n;
   file >> s >> userInput->n;
   file >> s;
   n = userInput->n;
   userInput->row = new int[n];
   userInput->matrix = new int*[n];
   for (int i = 0; i < n; ++i)
   {
      userInput->matrix[i] = new int[n];
   }
   
   for (int i = 0; i < n; ++i)
   {
      for (int j = 0; j < n; ++j)
      {
         userInput->matrix[i][j] = 0;
      }
   }

   userInput->column = new MinHeap<columnEntry*>(n+1,&isGreater);

   int rowSum = 0;
   int colSum = 0;
   for (int i = 0; i < n; ++i)
   {
      file >> userInput->row[i];
      rowSum += userInput->row[i];
   }
   file >> s;
   for (int i = 0; i < n; ++i)
   {
      columnEntry* x = new columnEntry;
      x->n = i;
      file >> x->val;
      colSum += x->val;

      userInput->column->insertKey(x);
   }
   if (rowSum != colSum)
   {
      cout << "Not Possible" << endl;
      exit(0);
   }

   return;
}

void GreedyAlgo(given* userInput){
   int n = userInput->n;
   MinHeap<columnEntry*> *heap = userInput->column;
   int** board = userInput->matrix;

   for (int i = 0; i < n; ++i)
   {
      int pieces = userInput->row[i];
      vector<columnEntry*> myValues;

      for (int j = 0; j < pieces; ++j)
      {
         columnEntry* col = heap->extractMin();
         if (col && col->val && board[i][col->n] == 0)
         {
            cout << "( " << i+1 << "," << col->n+1 << " )   ";
            board[i][col->n] = 1;
            col->val--;
            myValues.push_back(col);
         } else if(col && col->val){
            j--;
            myValues.push_back(col);
         } else {
            cout << "Not Possible" << endl;
            return;
         }
      }
      for (unsigned int j = 0; j < myValues.size(); ++j)
      {
         heap->insertKey(myValues[j]);
      }
      cout << endl;
   }
   return;
}


int main(){
   given* userInput = new given;
   string fileName;
   cout << "Enter File Name: ";
   cin >> fileName;
   ReadInput(userInput,fileName);
   GreedyAlgo(userInput);
   return 0;
}
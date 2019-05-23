/*******************************************************************************************************
*Name : Abdul Manan
*Roll# 2020-10-0198
*
*Algorithm:
* First of all , Sort all racks by L/P where L is Length and P is probability
*    PIck minimum one and place it.
*    Repeat above step until all are placed
* 
*Time Complexity:
*  O(n.log.n)
*Space Complexity:
* O(n) where n is number of racks
********************************************************************************************************/


#include <bits/stdc++.h>
#include "heap.cpp"
using namespace std;

struct columnEntry{
   int n;
   int val;
};

struct Rack{
   double Length;
   double Prob;
   double Expec;
   int rNum;
};
struct given{
   int n;
   vector<Rack*> allRacks;
};


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

  // userInput->allRacks = new Rack[n];


   

//   file >> s;
   char c;
   for (int i = 0; i < n; ++i)
   {
      Rack* x = new Rack;
      x->rNum = i+1;
      file >> x->Length >> c;
      userInput->allRacks.push_back(x);
   }

   //file >> s;
   for (int i = 0; i < n; ++i)
   {
      file >> userInput->allRacks[i]->Prob >> c;
   }

   return;
}
bool comp(Rack *a, Rack *b){
   return a->Expec < b->Expec;
}
void GreedyAlgo(given* userInput){
   int n = userInput->n;

   vector<Rack*> racks = userInput->allRacks;
   for (int i = 0; i < n; ++i)
   {
      racks[i]->Expec = (1/racks[i]->Prob )* racks[i]->Length;
   }

   sort(racks.begin(),racks.begin()+userInput->n, &comp);
   double Expectation = 0;
   double sum =0;
   for (int i = 0; i < n; ++i)
   {
      cout << "Rack" << racks[i]->rNum << "\t";
      sum += racks[i]->Length;
      Expectation += racks[i]->Prob * sum;
   }
   cout << endl;
   cout << "Expectation: " << Expectation << endl;
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

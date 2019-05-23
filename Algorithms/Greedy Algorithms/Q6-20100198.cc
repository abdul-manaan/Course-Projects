/*******************************************************************************************************
*Name : Abdul Manan
*Roll# 2020-10-0198
*
*Algorithm:
* First of all store all Data in vectors. Like I store every Line of Input in one array
* Now I also store lines of each variable in a single vector
* Now, all I have to do is start of vector whose size is 0 and go on. Deleting every time a variable from all 
*  its lines
* 
*Complexity:
*  O(n) as my algorithm only traverse each variable, in one row, once.
*   n is number of args in all rows.
********************************************************************************************************/





#include <bits/stdc++.h>

using namespace std;



struct given{
   int n;
   vector<int> start;
   vector<string> checker;
   vector<int> *eachLine;
   vector<string> end;
   vector<string> *allLines;
};


vector<string> splitStr(string line){
        int n = line.length();
        int start = 0;
        vector<string> result;
        for (int i = 0; i < n; ++i) {
                if( !isalnum(line[i]) ) {
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

void ReadInput(given* userInput, string fileName){
   ifstream file;
   file.open(fileName.c_str());
   if (!file) {
      cout << "Unable to open file file.txt" << endl;
      return;   // call system to stop
   }

   int n;
   string str;
   getline(file,str);
   if (splitStr(str).size() == 2)
   {
      n = stoi(splitStr(str)[1]);
      userInput->n = n;
   }else {
      cout << "Format Error" << endl;
      exit(0);
   }

   userInput->eachLine = new vector<int>[n+1];
   userInput->allLines = new vector<string>[n+n+n];


   int lineNumber = 0;
   while(getline(file,str)){

      if (str.find("=>") != string::npos) {
         vector<string> line = splitStr(str);
         if (line.size() == 1)
         {
            userInput->start.push_back(lineNumber);
            userInput->end.push_back(line[0]);

         } else {
            
            for (int i = 0; i < line.size()-1; ++i)
            {
               userInput->allLines[lineNumber].push_back(line[i]);
               userInput->eachLine[getNum(line[i])].push_back(lineNumber);   
            }            
            userInput->end.push_back(line[line.size()-1]);
         }
         lineNumber++;      
      } else {
         userInput->checker.push_back(str);
      }
   }


}

void doesSatisfy(given* userInput){

   int n = userInput->n;
   vector<int> trueArgs;
   int size = userInput->start.size();
   while(size){
      int strt = userInput->start.front();
      userInput->start.pop_back();
      int srt = getNum(userInput->end[strt]);
      trueArgs.push_back(srt);
      for (int i = 0; i < userInput->eachLine[srt].size(); ++i)
      {
         int line = userInput->eachLine[srt][i];
         if (find(userInput->allLines[line].begin(),userInput->allLines[line].end(),"x"+to_string(srt) ) != userInput->allLines[line].end())
         {
            userInput->allLines[line].erase(find(userInput->allLines[line].begin(),userInput->allLines[line].end(),"x"+to_string(srt) ));
         }
         if (userInput->allLines[line].size() == 0)
         {
            userInput->start.push_back(line);
         }
      }
      userInput->eachLine[srt].clear();

      size = userInput->start.size();
   }

   bool isTrue = false;
   for (int i = 0; i < userInput->checker.size(); ++i)
   {
      vector<string> str = splitStr(userInput->checker[i]);
      for (int i = 0; i < str.size(); ++i)
      {  
         if(str[i][0] != 'x' ){

         } else {
            
            if (find(trueArgs.begin(),trueArgs.end(),getNum(str[i])) == trueArgs.end())
            {
               isTrue = true;
            }
            
         } 
         
      }
   }
   for (int i = 0; i < userInput->n; ++i)
   {
      if (find(trueArgs.begin(),trueArgs.end(),i+1) == trueArgs.end())
      {
         cout << "x" << i+1 << " " << 0 << endl;
      }else
      cout << "x" << i+1 << " " << 1 << endl;

   }
   if (!isTrue)
   {
      cout << "Not Satisfiable" << endl;
      return;
   }
   cout << "Satisfiable " << endl;   
}


void Print(given* userInput){
   for (int i = 0; i < 5; ++i)
   {
      for (int j = 0; j < userInput->eachLine[i].size(); ++j)
      {
         cout << userInput->eachLine[i][j] << " ";
      }
      cout << endl;
   }
}

int main(){
   given* userInput = new given;
   string fileName;
   cout << "Enter File Name: ";
   cin >> fileName;
   ReadInput(userInput,fileName);
  // Print(userInput);
   doesSatisfy(userInput);

   return 0;
}
/*************************************************************************************************************************
* Name : Abdul Manan
* Roll No. 2020-10-0198
* Algorithm:
*  ->  This code takes a Graph and tells if the set of n Leaders are safe to sit in rows or not.
*  ->  First of all, reads the Graph from the File and stores it in List Data Structure. 
*  ->  Then, We have to check if there is any Cycle in the Graph or not. If there is any, that means the given Leaders 
*      can not sit in rows. 
*  ->  In order to find the safe ordering, We will use Topological Sort and set level at each node. IF at any point, we find a node that is in process that will mean
*	   there is a cycle in the Graph and there is no Safe Ordering of the Leaders. 
*  ->  After finding an odd length cycle, we are sure that there is no safe ordering. So, just print the cycle by traversing backwards.
*  ->  if there is no cycle, at the end of above traversal, we will have a queue that contains safe ordering of the Leaders. We will just sit them according to
*	   there level number.
* 
* Time Complexity:
*     Lets assume there are n Nodes and m edges in the Graph.
*     -> Reading the Graph takes O(m+n) time.
*	  -> Graph Traversal also takes O(m + n) time as we have to traverse each node and check all its edges.
*     -> Printing the Cycle also takes O( z+1 ) time where z is number of Nodes in cycle which in worst case is O(n).
*     -> Putting Leaders' names in a Queue and printing them take n*O(1) = O(1) time as we have to check every Node.
*		So, overall time complexity is,
*       	O( m + n )
*
*Space Complexity:
*     SToring all Nodes and all edges in a List takes O(m+n) Space.
*  
* Test Files: test2b1.txt , test2b2.txt, test2b3.txt    
*   
****************************************************************************************************************************/

#include<iostream>
#include<fstream>
#include<queue>
#include<stack>
#include<map>
#include<vector>
#include<list>
#include <sstream>
using namespace std;



struct chemical;                           // Struct that Represents each Chemical Compund
vector<string> splitLine(string line);     // Helper Function to parse each Line
int indexOf(string s);                     //  Helper Function
void readGraph(list<chemical*>* myGraph, string fileName);   // Reads Graph from a File
void printGraph(list<chemical*> *myGraph);                   // Print the Graph
void safeSitting(list<chemical*>* myGraph);                  // Apply Topoological Sort
void freeTheSpace(list<chemical*>* myGraph);                 // free the Heap Memory





int main(){
	string fileName;
	getline(cin,fileName);
	list<chemical*>* myGraph = new list<chemical*>;
	readGraph(myGraph,fileName);
	// printGraph(myGraph);
	safeSitting(myGraph);
	freeTheSpace(myGraph);
}








/******************************************************************************************************************************/
/**********************************     FUNCTION DEFINITIONS                                   ********************************/
/******************************************************************************************************************************/






struct chemical{
	string name;
	int level;
	int isVisited;
	list<chemical*> edges;
	chemical* visitedBy;
};


vector<string> splitLine(string line){
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


int indexOf(string s){
	int i = 0;
	for (i = 0; i < s.length(); ++i){
		if(isdigit(s[i])){
			break;
		}
	}
	return stoi(s.substr(i,s.length()-i));
}


void readGraph(list<chemical*>* myGraph, string fileName){
	ifstream file;
	file.open(fileName.c_str());
	char x;
	int numOfChem;

	string eachLine;
	//file >> x >> numOfChem;
	getline(file,eachLine);
	stringstream ss;
	ss << eachLine;
	ss >> x >> numOfChem;
	//numOfChem = 6;
	chemical* chemArray[numOfChem];
	int n = 0;
	for (int i = 0; i < numOfChem; ++i){
		chemArray[i] = new chemical;
		chemArray[i]->isVisited = 0;
	}

	while(getline(file,eachLine)){
		vector<string> allWords = splitLine(eachLine);
		int n = indexOf(allWords[0]);
		chemArray[n]->name = allWords[0];
		for (int i = 1; i < allWords.size(); i++){
			int m = indexOf(allWords[i]);
			chemArray[n]->edges.push_back(chemArray[m]);	
		} 
	}
	for (int i = 0; i < numOfChem; ++i){
		myGraph->push_back(chemArray[i]);
	}

}

void printGraph(list<chemical*> *myGraph){
	list<chemical*>::iterator it = myGraph->begin();
	list<chemical*>::iterator it2;
	for (int i=0; i< myGraph->size(); i++, ++it){
		cout << (*it)->name << " : ";
		int j=0;
		for (it2 = (*it)->edges.begin(); j < (*it)->edges.size(); ++it2,++j){
			cout << (*it2)->name << " ";
		}
		cout << endl;
	}
}


void safeSitting(list<chemical*>* myGraph){
	int size = myGraph->size();
	queue<chemical*> myQueue;
	map<chemical*, int> myMap;
	list<chemical*>::iterator it = myGraph->begin();

	while(it != myGraph->end()){
		list<chemical*>::iterator ls = (*it)->edges.begin();
		if(!myMap[*it]){
			myMap[*it] = 0;
		}
		while(ls != (*it)->edges.end()){
			myMap[(*ls)] = myMap[(*ls)] + 1;
			ls++;
		}
		(*it)->level = 0;
		it++;
	}
	map<chemical*,int>::iterator mapIt = myMap.begin();
	int levelNum = 1;
	while(mapIt != myMap.end()){
		if(mapIt->second == 0){
			myQueue.push(mapIt->first);
			mapIt->first->level  = levelNum;
		}
		mapIt++;
	}
	levelNum++;

	queue<chemical* > result; 
	while(!myQueue.empty()){
		chemical* temp = myQueue.front();
		temp->isVisited = 1;
		myMap[temp] = -1;
		result.push(temp);
		it = temp->edges.begin();
		bool isLevelChanged = false;
		while(it != temp->edges.end()){
			if((*it)->isVisited != 1){
				myMap[(*it)] = myMap[(*it)] - 1;
				if (myMap[(*it)] == 0){
					myQueue.push((*it));
					(*it)->level = levelNum;
					isLevelChanged = true;
				}
			}
			it++;
		}
		if (isLevelChanged)
			levelNum++;
		myQueue.pop();
	}

	if (result.size() == size){
		cout << "Yes" << endl;
		cout << levelNum-1 << endl;
		int n = 1;
		while(!result.empty()){
			if(result.front()->level -1 == n){
				n++;
				cout << endl;
			}
			cout << result.front()->name << " ";
			result.pop();
		}
		cout << endl;
	} else {
		cout << "NO" << endl;
		//printCycle(myGraph);
	}
}



void freeTheSpace(list<chemical*>* myGraph){
	list<chemical*>::iterator it;
	for (it = myGraph->begin(); it != myGraph->end();  ++it)
	{
		delete (*it);
	}
	myGraph->clear();
	return;
}

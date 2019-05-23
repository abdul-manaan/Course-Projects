/*************************************************************************************************************************
* Name : Abdul Manan
* Roll No. 2020-10-0198
* Algorithm:
*  ->  This code takes a Graph and tells if the set of n Chemicals are safe to pack in two Boxes or not.
*  ->  First of all, reads the Graph from the File and stores it in List Data Structure. 
*  ->  Then, We have to check if there is any Odd Length Cycle in the Graph or not. If there is any, that means the given Chemicals 
*      can not be packed in two boxes. 
*  ->  In order to find the odd length cycle, start traversing the Graph from any Node and mark each Node that is visited as well as
*      store its level number as well. As soon as youu will find a node that is already visited and is on same level as of its parent Node,
*	   that means there is an Odd Length Cycle. 
*  ->  After finding an odd length cycle, we are sure that there is no safe ordering. So, just print the cycle by traversing backwards.
*  ->  if there is no cycle, at the end of above traversal, every node will have a level number. We can put odd level Chemicals  in one box 
*      and others in second box.
* 
* Time Complexity:
*     Lets assume there are n Nodes and m edges in the Graph.
*     -> Reading the Graph takes O(m+n) time.
*	  -> Graph Traversal also takes O(m + n) time as we have to traverse each node and check all its edges.
*     -> Printing the Cycle also takes O( z+1 ) time where z is number of Nodes in cycle which in worst case is O(n).
*     -> Putting all Checmicals in sepate Boxes take O(n) time as we have to check every Node.
*		So, overall time complexity is,
*       	O( m + n )
*
*Space Complexity:
*     SToring all Nodes and all edges in a List takes O(m+n) Space.
*     
* Test Files: test11.txt , test12.txt, test13.txt    
*
****************************************************************************************************************************/

#include<iostream>
#include<fstream>
#include<queue>
#include<stack>
#include<vector>
#include<list>
#include<sstream>
using namespace std;


struct chemical;                           // Struct that Represents each Chemical Compund
vector<string> splitLine(string line);     // Helper Function to parse each Line
int indexOf(string s);                     //  Helper Function
void readGraph(list<chemical*>* myGraph, string fileName);   // Reads Graph from a File
void printGraph(list<chemical*> *myGraph);                   // Print the Graph
void isSafeToPack(list<chemical*>* myGraph);                 // Checks if possible to Pack Chemicals
void freeTheSpace(list<chemical*>* myGraph);                 // free the Heap Memory


int main(){
	string fileName;
	getline(cin,fileName);
	list<chemical*>* myGraph = new list<chemical*>;
	readGraph(myGraph,fileName);
	// printGraph(myGraph);
	isSafeToPack(myGraph);
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
		chemArray[i]->level = -1;
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

void isSafeToPack(list<chemical*>* myGraph){
	queue<chemical*> myQueue;
	queue<chemical*> result;
	queue<chemical*> result1;

	myGraph->front()->level = 1;
	myGraph->front()->isVisited = 1;
	myGraph->front()->visitedBy = NULL;
	myQueue.push(myGraph->front());

	while(!myQueue.empty()){
		chemical* chem = myQueue.front();
		result.push(chem);
		myQueue.pop();
		list<chemical*>::iterator it = chem->edges.begin();
		for(;it != chem->edges.end(); ++it){
			if( (*it)->isVisited == 0) {
				(*it)->level  = chem->level+1;
				(*it)->isVisited = 1;
				(*it)->visitedBy = chem;
				myQueue.push((*it));
			} else if( (*it)->level == chem->level) {
				cout << "No" << endl;
				chemical* temp1 = chem;
				chemical* temp2 = *it;
				stack<chemical*> s;
				cout << temp2->name << " -> ";
				while(temp1->name != temp2->name){
					s.push(temp2);
					cout << temp1->name << " -> ";
					temp1 = temp1->visitedBy;
					temp2 = temp2->visitedBy;
				}
				cout << temp1->name;

				while(!s.empty()){
					cout << " -> " << s.top()->name;
					s.pop();
				}
				return;
			}
		}
	}
	cout << "Yes" << endl;
	while(!result.empty()){
		if (result.front()-> level % 2){
			cout << result.front()->name << " ";
		} else {
			result1.push(result.front());
		}
		result.pop();
	}
	cout << endl;
	while(!result1.empty()){
		cout << result1.front()->name << " ";
		result1.pop();
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
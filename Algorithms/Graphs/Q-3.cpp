/*************************************************************************************************************************
* Name : Abdul Manan
* Roll No. 2020-10-0198
* Algorithm:
*  ->  This code takes a Graph and tells if the shortest distance from C​A to C​B via C​x increases by no more than 20% of the shortest                  
*	   distance from C​A​ to C​B​. 
*  ->  First of all, reads the Graph from the File and stores it in List Data Structure. 
*  ->  Then, We have to apply Dijkstra between 3 sets of cities Ca and Cb, Ca and Cx and Cx and Cb. 
*  ->  We can simply get the distance of a path as wll as complete path from the Dijkstra. So, juts find above paths and their distances
*	   and calculate if Can o Cb via Cx is more than 20% or not.
* 
* Time Complexity:
*     Lets assume there are n Nodes and m edges in the Graph.
*     -> Reading the Graph takes O(m+n) time.
*	  -> Dijkstra takes O(m Log n) time.
*     -> Three times Dijkstra just adds a constant 3 to aove time.
*		So, overall time complexity is,
*       	O( m Log n )
*
* Space Complexity:
*     SToring all Nodes and all edges in a List takes O(m+n) Space.
*
* Test Files: test31.txt , test32.txt, test33.txt    
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
#include <ctype.h>
#include "heap.cpp"
using namespace std;

struct chemical;
struct edge;
void printGraph(list<chemical*> *myGraph);
vector<string> splitLine(string line);
int indexOf(string s);
void readGraph(list<chemical*>* myGraph, vector<edge*>& allEdges, string fileName);
list<chemical*> dijkstra(list<chemical*>* myGraph, vector<edge*> allEdges, string source, string desti);
void task3(list<chemical*>* myGraph, vector<edge*> allEdges);
void freeTheSpace(list<chemical*>* myGraph);




int main(){
	string fileName;
	getline(cin,fileName);
	list<chemical*>* myGraph = new list<chemical*>;
	vector<edge*> allEdges;
	readGraph(myGraph,allEdges, fileName);
	task3(myGraph,allEdges);
	freeTheSpace(myGraph);
}



/******************************************************************************************************************************/
/**********************************     FUNCTION DEFINITIONS                                   ********************************/
/******************************************************************************************************************************/




struct chemical{
	string name;
	list<edge*> edges;
	int dist;
	int isVisited;
	chemical* parent;
};

struct edge{
	int weight;
	chemical* source;
	chemical* dest;
};





void printGraph(list<chemical*> *myGraph){
	list<chemical*>::iterator it = myGraph->begin();
	list<edge*>::iterator it2;
	for (int i=0; i< myGraph->size(); i++, ++it){
		cout << (*it)->name << " : ";
		int j=0;
		for (it2 = (*it)->edges.begin(); j < (*it)->edges.size(); ++it2,++j){
			cout << (*it2)->dest->name << " ";
		}
		cout << endl;
	}
}

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
bool compNode(chemical* a, chemical* b){
    return a->dist < b->dist;
}

void readGraph(list<chemical*>* myGraph, vector<edge*>& allEdges, string fileName){
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
	}

	while(getline(file,eachLine)){
		vector<string> allWords = splitLine(eachLine);
		int n = indexOf(allWords[0]);
		chemArray[n]->name = allWords[0];
		for (int i = 1; i < allWords.size(); i+=2){
			edge* newEdge = new edge;
			int m = indexOf(allWords[i]);
			newEdge->source = chemArray[n];
			newEdge->dest   = chemArray[m];
			newEdge->weight = stoi(allWords[i+1]);	
			allEdges.push_back(newEdge);
			chemArray[n]->edges.push_back(newEdge);	
		} 
	}
	for (int i = 0; i < numOfChem; ++i){
		myGraph->push_back(chemArray[i]);
	}

}

void printEdges(vector<edge*> allEdges){
	for (int i = 0; i < allEdges.size(); ++i)
	{
		cout << allEdges[i]->source->name << " " << allEdges[i]->dest->name << " " << allEdges[i]->weight << endl;
	}
}

struct inputStruct{
	string Ca;
	string Cb;
	string Cx;
};

list<chemical*> dijkstra(list<chemical*>* myGraph, vector<edge*> allEdges, string source, string desti){
	MinHeap<chemical*> myHeap(myGraph->size()+10, &compNode);
	list<chemical*>::iterator it;
	for (it = myGraph->begin(); it != myGraph->end(); ++it){
		if( (*it)->name == source){
			(*it)->dist = 0;
			(*it)->parent = NULL;
		} else {
			(*it)->dist = INT_MAX;
		}
		myHeap.insertKey((*it));
	}

	chemical* res;
	while(!myHeap.isEmpty()){
		//myHeap.print_heap();
		chemical* min = myHeap.extractMin();
		// /myHeap.print_heap();
		if(min->name == desti) {
			res = min;
			break;
		}
		list<edge*>::iterator it;
		for (it = min->edges.begin(); it != min->edges.end(); ++it) {
			if ((*it)->dest->dist > (*it)->weight + min->dist) {
				(*it)->dest->parent = min;
				(*it)->dest->dist = (*it)->weight + min->dist;
				// cout << (*it)->dest->dist << endl;
				myHeap.decreaseKey(myHeap.getIndex((*it)->dest), (*it)->dest);
			}
		}
	}
	list<chemical*> result;
	cout << res->dist << endl;
	while(res != NULL){
		chemical* x = new chemical;
		x->name = res->name;
		x->dist = res->dist;
		x->parent = res->parent;
		result.push_front(x);
		res = res->parent;
	}
	return result;
}

void task3(list<chemical*>* myGraph, vector<edge*> allEdges){
	inputStruct in;
	cout << "Enter Source Ca: ";
	cin >> in.Ca;
	cout << "Enter Destination Cb: ";
	cin >> in.Cb;
	cout << "Enter Scenic Point Cx: ";
	cin >> in.Cx;
	list<chemical*> result1 = dijkstra(myGraph,allEdges,in.Ca, in.Cb);
	list<chemical*> result2 = dijkstra(myGraph,allEdges,in.Ca, in.Cx);
	list<chemical*> result3 = dijkstra(myGraph,allEdges,in.Cx, in.Cb);	

	float Diff =  ( ( result2.back()->dist + result3.back()->dist ) - result1.back()->dist * 1.0) / result1.back()->dist * 100.0 ;
	if( Diff > 20) {
		cout << "I should not visit " << in.Cx << " becasue the distance via this is " << Diff << "% more than the Shortest Path" << endl;
	} else {
		cout << "I should visit " << in.Cx << " becasue the distance via this is only " << Diff << "% more than the Shortest Path" << endl;
	}
	cout << "From " << in.Ca << " to " << in.Cb << endl;
	cout << "\tLength : " << result1.back()->dist << endl;
	cout << "\tPath: ";

	list<chemical*> ::iterator it;

	it = result1.begin();
	cout << (*it)->name;
	it++;
	for (; it != result1.end(); ++it){
		cout << " -> " << (*it)->name;	 
	}
	cout << endl;
	
	cout << "From " << in.Ca << " to " << in.Cb << " via " << in.Cx << endl;
	cout << "\tLength : " << result2.back()->dist + result3.back()->dist << endl;
	cout << "\tPath : ";

	it = result2.begin();
	cout << (*it)->name;
	it++;
	for (; it != result2.end(); ++it){
		cout << " -> " << (*it)->name;	 
	}

	it = result3.begin();
	it++;
	for (; it != result3.end(); ++it){
		cout << " -> " << (*it)->name;	 
	}
	freeTheSpace(&result1);
	freeTheSpace(&result2);
	freeTheSpace(&result3);
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

/*******************************************************************************************************
*Name : Abdul Manan
*Roll# 2020-10-0198
*
*Algorithm:
* First of all , make MST using Prim's Algorithm
*   Add the given edge
*   Find Cycle in Graph and reomve the edge from that cycle with max Weight
*  
* 
*Time Complexity:
*  O(E) 
*Space Complexity:
* O(N+E) where N is number of nodes and E is edges
********************************************************************************************************/


#include <bits/stdc++.h>
#include "heap.cpp"
using namespace std;

struct Node;
struct Edge;
struct Node{
   string Name;
   vector<Edge*> myEdges;
   int isVisited;
};
struct Edge{
   int Weight;
   Node* nodeA;
   Node* nodeB;
};

struct given{
   int n;
   vector<Node*> allNodes;
   vector<Edge*> allEdges;
   vector<Edge*> unusedEdges;
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

int getNodeNum(string s){
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

Edge* isEdgePresent(vector<Edge*> allEdges, Edge*x){
   for (unsigned int i = 0; i < allEdges.size(); ++i)
   {
      if(allEdges[i]->Weight == x->Weight){
         if (allEdges[i]->nodeA == x->nodeB && allEdges[i]->nodeB == x->nodeA)
         {
            return allEdges[i];
         }
      }
   }
   return NULL;
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


   for (int i = 0; i < n; ++i)
   {
      Node* x = new Node;
      userInput->allNodes.push_back(x);
   }



   while(getline(file,str)){
      vector<string> allParts = splitStr(str);
      int nodeNum = getNodeNum(allParts[0]);
      userInput->allNodes[nodeNum]->Name = allParts[0];

      for (unsigned int i = 1; i < allParts.size(); i+=2)
      {
         Edge* x = new Edge;
         x->Weight = stoi(allParts[i+1]);

         x->nodeA = userInput->allNodes[nodeNum];
         x->nodeB = userInput->allNodes[getNodeNum(allParts[i])]; 
         if (isEdgePresent(userInput->allEdges, x) != NULL)
         {
           Edge* y = isEdgePresent(userInput->allEdges, x);
           delete x;
           x = y;
         }else {
            userInput->allEdges.push_back(x);
         }
         userInput->allNodes[nodeNum]->myEdges.push_back(x);
      }
   }


}
bool comp(Edge* a, Edge* b){
   return a->Weight < b->Weight;
}

template <typename T>
set<T> getUnion(const set<T>& a, const set<T>& b)
{
  set<T> result = a;
  result.insert(b.begin(), b.end());
  return result;
}

set<Node*> findSet(vector< set<Node*> > tree, Node* node ){
   for (unsigned int i = 0; i < tree.size(); ++i)
   {
      for (set<Node*>::iterator it = tree[i].begin(); it != tree[i].end(); ++it)
      {
         if ((*it)->Name == node->Name)
         {
            return tree[i];
         }
      }
   }
   set<Node*> a;
   return a;
}

given* MST(given* userInput){
   vector< set<Node*> > tree;
   vector<Edge*> treeBranch;

   int n = userInput->n;
   vector<Node*> allNodes = userInput->allNodes;
   vector<Node*> newNodes;
   vector<Edge*> allEdges = userInput->allEdges;

   for (int i = 0; i < n; ++i)
   {
      Node* x = new Node;
      x->Name = allNodes[i]->Name;
      newNodes.push_back(x);
      set<Node*> set1;
      set1.insert(x);
      tree.push_back(set1);
   }

   MinHeap<Edge*> myHeap(allEdges.size(),&comp);
   for (unsigned int i = 0; i < allEdges.size(); ++i)
   {
      myHeap.insertKey(allEdges[i]);
   }

   while(!myHeap.isEmpty()){
      Edge* minEdge = myHeap.extractMin();

      set<Node*> set1 = findSet(tree,minEdge->nodeA);
      set<Node*> set2 = findSet(tree,minEdge->nodeB);

      if (set1 != set2)
      {
         //Adding Edge in our MST
         Edge* x = new Edge;
         x->nodeA  =  newNodes[getNodeNum(minEdge->nodeA->Name)];
         x->nodeB  =  newNodes[getNodeNum(minEdge->nodeB->Name)];
         x->Weight =  minEdge->Weight;
         treeBranch.push_back(x);
         x->nodeA->myEdges.push_back(x);
         x->nodeB->myEdges.push_back(x);
         //Union of Two Sets
         set<Node*> result = getUnion(set1,set2);
         vector< set <Node*> > ::iterator it;
         it = find(tree.begin(), tree.end(),set1);
         tree.erase (it);
         it = find(tree.begin(), tree.end(),set2);
         tree.erase (it);
         tree.push_back(result);
      }
      else {
         Edge* x = new Edge;
         x->nodeA  =  newNodes[getNodeNum(minEdge->nodeA->Name)];
         x->nodeB  =  newNodes[getNodeNum(minEdge->nodeB->Name)];
         x->Weight =  minEdge->Weight;

         userInput->unusedEdges.push_back(x);
      }
   }

   given* result = new given;
   result->n = n;
   result->allEdges = treeBranch;
   result->allNodes = newNodes;
   result->unusedEdges = userInput->unusedEdges;
   


   //Printing Result

   cout << "MST: ";
   for (unsigned int i = 0; i < treeBranch.size(); ++i)
   {
      cout << "(" << treeBranch[i]->nodeA->Name << " , " << treeBranch[i]->nodeB->Name << ")   ";
   }
   cout << endl;

   return result;

}




int FindSndMST(given* userInput, unsigned int i = 0){


   return 0;

}


void PrintGraph(given* userInput){
   for (int i = 0; i < userInput->n; ++i)
   {
      cout << userInput->allNodes[i]->Name << " ";
      for (unsigned int j = 0; j < userInput->allNodes[i]->myEdges.size(); ++j)
      {
         cout << userInput->allNodes[i]->myEdges[j]->nodeA->Name << " ";
         cout << userInput->allNodes[i]->myEdges[j]->nodeB->Name << " ";
         cout << userInput->allNodes[i]->myEdges[j]->Weight << "\t";
      }
      cout << endl;
   }
}



int findEdgesInCycle(Node* nodeA, Node* nodeB, set<Edge*> &cycle){
   vector<Edge*> myEdges = nodeA->myEdges;
   if (nodeA->isVisited)
   {
      return 0;
   }
   //cout << nodeA->Name << " " << nodeB->Name << endl;
   nodeA->isVisited = 1;
   for (unsigned int i = 0; i < myEdges.size(); ++i)
   {
      if (myEdges[i]->nodeA == nodeA)
      {
         if (myEdges[i]->nodeB == nodeB)
         {
             cycle.insert(myEdges[i]);
             return 1;
         } else {
            if (findEdgesInCycle(myEdges[i]->nodeB, nodeB, cycle))
            {
               cycle.insert(myEdges[i]);
               return 1;
            }
         }
      } else  {
         if (myEdges[i]->nodeA == nodeB)
         {
             cycle.insert(myEdges[i]);
             return 1;
         } else {
            if (findEdgesInCycle(myEdges[i]->nodeA, nodeB, cycle))
            {
               cycle.insert(myEdges[i]);
               return 1;
            }
         }      
      }
   }
   return 0;
}
void addEdge(given* userInput, int a, int b, int w){

   //New Edge
   Edge* newEdge = new Edge;
   newEdge->nodeA = userInput->allNodes[a];
   newEdge->nodeB = userInput->allNodes[b];
   newEdge->Weight = w;


   //Print New Edge

   cout << "Edge: (" << newEdge->nodeA->Name << " , " << newEdge->nodeB->Name << ") : " << newEdge->Weight << endl;
   //Finding Cycle Nodes

   set<Edge*> cycle;
   for (unsigned int i = 0; i < userInput->allNodes.size(); ++i)
   {
      userInput->allNodes[i]->isVisited = 0;
   }
   findEdgesInCycle(newEdge->nodeA, newEdge->nodeB, cycle);
   //cout << cycle.size() << endl;

   //Finding Min Weight Edge
   int maxWeight = INT_MIN;
   Edge* minEdge;
   for (set<Edge*>::iterator i = cycle.begin(); i != cycle.end(); ++i)
   {
      if ((*i)->Weight > maxWeight)
      {
         maxWeight = (*i)->Weight;
         minEdge = *i;
      }
   }

   if (maxWeight > w)
   {
      //REmoving Previous Edge and ADd New
      userInput->allEdges.erase( find(userInput->allEdges.begin(), userInput->allEdges.end(), minEdge) );
      minEdge->nodeA->myEdges.erase( find( minEdge->nodeA->myEdges.begin(),minEdge->nodeA->myEdges.end(), minEdge));
      minEdge->nodeB->myEdges.erase( find( minEdge->nodeB->myEdges.begin(),minEdge->nodeB->myEdges.end(), minEdge));

      userInput->allEdges.push_back(newEdge);
      newEdge->nodeA->myEdges.push_back(newEdge);
      newEdge->nodeB->myEdges.push_back(newEdge);

   } else {
      cout << "No Change in MST" << endl;
   }


   //Printing
   cout << "MST: ";
   for (unsigned int i = 0; i < userInput->allEdges.size(); ++i)
   {
      cout << "(" << userInput->allEdges[i]->nodeA->Name << " , " << userInput->allEdges[i]->nodeB->Name << ")   ";
   }
   cout << endl;
}


int main(){
   given* userInput = new given;
   string fileName;
   cout << "Enter File Name: ";
   cin >> fileName;
   ReadInput(userInput,fileName);
   userInput = MST(userInput);
   // cout << "Nodes: " << userInput->allNodes.size() << endl;
   // cout << "Edges: " << userInput->allEdges.size() << endl;
   // PrintGraph(userInput);
   addEdge(userInput,1,3,1);  
  

   return 0;
}

#ifndef __HEAP_H
#define __HEAP_H

#include <iostream>
#include <climits>

using namespace std;

template<class T>
bool isLess(T a, T b){
   return a < b;
}
template<class T> 
class MinHeap
{
    T *harr; // pointer to array of elements in heap
    int capacity; // maximum possible size of min heap
    int heap_size; // Current number of elements in min heap
    bool (*comp)(T,T);
public:
    // Constructor

    MinHeap(int cap);
    MinHeap(int cap, bool (*compare)(T,T));
 
    // to heapify a subtree with root at given index
    void MinHeapify(int i);
 
    // return index of the parent of a node at index i
    int parent(int i);
 
    // get index of left child of node at index i
    int left(int i);
 
    // get index of right child of node at index i
    int right(int i);
 
    // remove minimum element (or root) from min heap
    T extractMin();
 
    // Decreases key value of key at index i to new_val 
    // (assume new_val will always be smaller than harr[i])
    void decreaseKey(int i, T new_val);
 
    // Returns the minimum key (key at root) from min heap
    T getMin();
    int getIndex (T val);
 
    // Deletes a key stored at index i
    void deleteKey(int i);
 
    // Inserts a new key 'k'
    void insertKey(T k);

    T* getHeap();

    bool isEmpty(){
        if (heap_size > 0) {
            return false;
        } else {
            return true;
        }
    }
    int size(){
        return heap_size;
    }
};

#endif

#ifndef __HEAP_CPP
#define __HEAP_CPP
#include "heap.h"
template<class T> 
void printHeap(T* arr, int x){
    for(int i =0; i< x; i++) {
        cout << arr[i]->name << endl;
    }
}

template<class T> 
MinHeap<T>::MinHeap(int cap)
{
    harr = new T[cap]();
    capacity = cap;
    heap_size = 0;  
    comp = &isLess;
}

template<class T> 
MinHeap<T>::MinHeap(int cap,bool (*compare)(T,T)) : comp(compare)
{
    harr = new T[cap]();
    capacity = cap;
    heap_size = 0;  
}

template<class T> 
void MinHeap<T>::MinHeapify(int i)
{
    if(i >= heap_size) return;
    int leftIndex  = this->left(i);   
    int rightIndex = this->right(i);
    int minimum;
    if (leftIndex < heap_size) {  
        if (rightIndex < heap_size) {  
            minimum = ( comp(harr[rightIndex] , harr[leftIndex]))? rightIndex : leftIndex;
        } else {
            minimum = leftIndex;   
        }
        if(comp(harr[minimum] , harr[i] )){
            swap(harr[minimum],harr[i]);
            MinHeapify(minimum);
        }
    }
    else {
        return;
    } 
}

 
template<class T> 
int MinHeap<T>::parent(int i)
{
    return (i-1)/2;
}
 
template<class T> 
int MinHeap<T>::left(int i)
{
    return (i*2+1);
}
 
template<class T> 
int MinHeap<T>::right(int i)
{
    return (i*2+2);
}
 
template<class T > 
T MinHeap<T>::extractMin()
{
    if(heap_size > 0){
        T res = harr[0];
        this->deleteKey(0);
        return res;
    }
    return NULL;
}
     
template<class T > 
void MinHeap<T>::decreaseKey(int i, T new_val)
{
	if(i >= heap_size){
		return;
	}
    harr[i] = new_val;
    int index = i;

    while(index){
    	int pare = this->parent(index);
    	if( comp( harr[index], harr[pare]) ){
    		swap(harr[pare],harr[index]);
 			index = pare;
    	}
    	else{
    		break;
    	}
    }
}
template<class T>
int MinHeap<T>::getIndex(T val){
    for(int i =0; i< heap_size; i++) {
        if( harr[i] == val) {
            return i;
        }
    }
    return heap_size+10;
} 

template<class T > 
T MinHeap<T>::getMin()
{
    return harr[0];
}

template<class T> 
void MinHeap<T>::deleteKey(int i)
{
	if(i < heap_size){
	    this->harr[i] = harr[heap_size-1];
	    heap_size--;
	    MinHeapify(i);
	}
}
 
template<class T > 
void MinHeap<T>::insertKey(T k)
{
	if(heap_size >= capacity){
		return;
	}
    harr[heap_size] = k;
    int index = heap_size;
    heap_size++;

    while(index){
    	int pare = this->parent(index);
    	if( comp( harr[index], harr[pare]) ){
    		swap(harr[pare],harr[index]);
 			index = pare;
    	}
    	else{
    		break;
    	}
    }

}

template<class T > 
T* MinHeap<T>::getHeap()
{
	return harr;
}

    

#endif

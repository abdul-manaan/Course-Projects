#ifndef PART_3
#define PART_3

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
enum DIRECTION {
    NORTH,
    SOUTH,
    EAST,
    WEST
};

enum LANE {
    LEFT,
    RIGHT
};

struct eachSignal{
	sem_t leftLane;
	sem_t shouldRGo;
	sem_t shouldSGo;
	int whereToGo; //1 for Left 2 for Straight
	int LeftWait;
	int RightWait;
	int isInside;
	pthread_mutex_t mutexL;// = PTHREAD_MUTEX_INITIALIZER;

};

void* SignalHandler();
void* SideController(void*);
void initializeP3();
void *goingFromToP3(void * argument);
void startP3();
#endif

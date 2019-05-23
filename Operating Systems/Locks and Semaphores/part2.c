#include <pthread.h>
#include "main.h"
#include "part2.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>

const int INTER_ARRIVAL_TIME = 5;
const int NUM_TRAINS = 5;
 
/**
 * Do any initial setup work in this function.
 * numStations: Total number of stations. Will be >= 5. Assume that initially
 * the first train is at station 1, the second at 2 and so on.
 * maxNumPeople: The maximum number of people in a train
 */


sem_t* waitingQueue;
sem_t** outWaiting;
int * numOfWaiters;    //Passengers waiting in  Boarding Area
int** numOfPassengers; //Passengers who want to get out of Train
int* trainState;

pthread_mutex_t mutexL = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
void* Handler(void* arg);


void initializeP2(int numStations, int maxNumPeople) {
	pthread_t pid[NUM_TRAINS];

	outWaiting = (sem_t**)malloc(sizeof(sem_t*)*NUM_TRAINS);
	for (int i = 0; i < NUM_TRAINS; ++i)
	{
		outWaiting[i] = malloc(sizeof(sem_t)*numStations);
	}

	waitingQueue = malloc(numStations*sizeof(sem_t));
	numOfWaiters = malloc(numStations*sizeof(int));
	trainState = malloc(numStations*sizeof(int));

	numOfPassengers = (int**)malloc(sizeof(int*)*NUM_TRAINS);
	for (int i = 0; i < NUM_TRAINS; ++i)
	{
		numOfPassengers[i] = malloc(sizeof(int)*numStations);
	}

	struct trainArgs *args = malloc(sizeof(struct trainArgs)*NUM_TRAINS);
	for (int i = 0; i < NUM_TRAINS; ++i)
	{
		args[i].numStations = numStations;
		args[i].maxNumPeople = maxNumPeople;
		args[i].startPoint = i;//(struct trainArgs){numStations,maxNumPeople,i};
		pthread_create(&pid[i], 0, Handler, (void*)(&args[i]));
	}


}

/**
 * Print in the following format:
 * If a user borads on train 0, from station 0 to station 1, and another boards
 * train 2 from station 2 to station 4, then the output will be
 * 0 0 1
 * 2 2 4
 */
void *goingFromToP2(void *argu) {
    struct argument *args = (struct argument*)(argu);
	
	int user_id = args->id;
	int fromStation = args->from;
	int toStation  = args->to;
	fflush(stdout);	

	pthread_mutex_lock(&mutexL);
	numOfWaiters[fromStation]++;
	pthread_mutex_unlock(&mutexL);

	sem_wait(&waitingQueue[fromStation]);
	//printf("-> %d %d %d -- %d\n",user_id,fromStation,toStation,trainState[fromStation] );

	pthread_mutex_lock(&mutex2);
	int trainID = trainState[fromStation];
	numOfPassengers[trainState[fromStation]][toStation]++;
	pthread_mutex_unlock(&mutex2);

	sem_wait(&outWaiting[trainState[fromStation]][toStation]);
	printf("%d %d %d\n",trainID,fromStation,toStation );
	fflush(stdout);
}

void startP2(){}
void* Handler(void* arg){
	struct trainArgs* args = (struct trainArgs*)arg;
	int trainID = args->startPoint;
	int curStation = args->startPoint;
	int n = 0;
	sleep(1);
	while(1){
		//printf("Train %d is at station %d\n",trainID, curStation);
		//fflush(stdout);
		trainState[curStation] = trainID;
		if (numOfPassengers[trainID][curStation]) {
			while(numOfPassengers[trainID][curStation]){
				sem_post(&outWaiting[trainID][curStation]);
				numOfPassengers[trainID][curStation]--;
				n--;
			}
		}
		pthread_mutex_lock(&mutexL);
		if (numOfWaiters[curStation]) {
			while(numOfWaiters[curStation] && n < args->maxNumPeople){
				sem_post(&waitingQueue[curStation]);
				numOfWaiters[curStation]--;
				n++;
			}
		}
		pthread_mutex_unlock(&mutexL);

		curStation = (++curStation)%(args->numStations);
		sleep(INTER_ARRIVAL_TIME-4);
	}
}
#include "part1.h"
#include <pthread.h>
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

/**
 * Do any initial setup work in this function.
 * numFloors: Total number of floors elevator can go to
 * maxNumPeople: The maximum capacity of the elevator
 */

sem_t* semToFloor;
sem_t* semFromFloor;
int*  destination;
int*  source;
int LiftCurCap = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* LiftHandler(void* arg);


void initializeP1(int numFloors, int maxNumPeople) {
	struct liftInitializer* args = malloc(sizeof(struct liftInitializer));
	args->maxNumPeople = maxNumPeople;
	args->numFloors = numFloors;
	
	semFromFloor = malloc(sizeof(sem_t)*numFloors);
	semToFloor = malloc(sizeof(sem_t)*numFloors);
	destination = malloc(sizeof(int)*numFloors);
	
	for (int i = 0; i < numFloors; ++i)
	{
		destination[i] = 0;
	}
	source = malloc(sizeof(int)*numFloors);
	
	for (int i = 0; i < numFloors; ++i)
	{
		source[i] = 0;
	}	
	pthread_t lift;
	pthread_create(&lift, NULL, LiftHandler, (void*)args);
}

/**
 * This program should print data in the specific format
 * described below:
 * If there is a passenger (id 5) going from floor 2 to 4, followed
 * by a passenger (id 6) from 2 to 5, the output should be:
 * 5 2 4
 * 6 2 5
 * i.e. there should be a single space, and each journey should be 
 * on a new line. Incorrectly formatted output will get 0 marks
 */
void* goingFromToP1(void *arg) {
    struct argument *args = (struct argument *) arg;
    //printf("%d %d %d\n", temp->id, temp->from, temp->to);
	int user_id = args->id;
	int fromFloor = args->from;
	int toFloor  = args->to;

	pthread_mutex_lock(&mutex);
	source[fromFloor]++;
	pthread_mutex_unlock(&mutex);

	sem_wait(&semFromFloor[fromFloor]);

	//printf("-> %d %d %d\n", user_id,fromFloor,toFloor);
	//fflush(stdout);
	pthread_mutex_lock(&mutex);
	destination[toFloor]++;
	pthread_mutex_unlock(&mutex);

	sem_wait(&semToFloor[toFloor]);

	printf("%d %d %d\n", user_id,fromFloor,toFloor);
	fflush(stdout);

}

void startP1(){}
void* LiftHandler(void* arg){

	struct liftInitializer* args = (struct liftInitializer*)arg;
	
	int curFloor = 0;
	int LiftGoingUp = 1;
	sleep(2);
	//printf("Lift Started.\n");
	while(1){
		///printf("Lift is at Floor %d \n",curFloor);
		//fflush(stdout);
		sleep(1);

		if (destination[curFloor])
		{
			while(destination[curFloor]){
				destination[curFloor]--;
				LiftCurCap--;
				sem_post(&semToFloor[curFloor]);
			}
		}

		if (source[curFloor])
		{
			while(source[curFloor] && LiftCurCap < args->maxNumPeople){
				LiftCurCap++;
				source[curFloor]--;
				sem_post(&semFromFloor[curFloor]);
			}
		}
		(LiftGoingUp) ? curFloor++ : curFloor--;
		if (curFloor == args->numFloors-1)
		{
			LiftGoingUp = 0;

		} else if(curFloor == -1){
			curFloor = 1;
			LiftGoingUp = 1;
		}
		//sleep(1);
		
	}
}
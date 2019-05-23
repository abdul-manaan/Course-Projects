#include <pthread.h>
#include "main.h"
#include "part3.h"
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


sem_t signal;
sem_t signalAck;
int MAX_PASS = 2 ;
enum DIRECTION TrafficLight;;//[4];
enum DIRECTION tellMeDirection[] = {NORTH,SOUTH,EAST,WEST};
struct eachSignal signalData[4];



void initializeP3() {
	pthread_t pid;
	pthread_create(&pid,NULL,SignalHandler,NULL);

	for (int i = 0; i < 4; ++i)
	{
		signalData[i].LeftWait = 0;
		signalData[i].whereToGo = 0;
		signalData[i].RightWait = 0;
		signalData[i].isInside = 0;
	}
	int *x = malloc(sizeof(int)*4);
	memcpy(x,(int[4]){0,1,2,3},sizeof(int)*4);
	for (int i = 0; i < 4; ++i)
	{
		pthread_t pid;
		pthread_create(&pid,NULL,SideController,(void*)(&x[i]));
	}
}
/**
 * If there is a car going from SOUTH to NORTH, from lane LEFT,
 * print 
 * SOUTH NORTH LEFT
 * Also, if two cars can simulateneously travel in the two lanes,
 * first print all the cars in the LEFT lane, followed by all the
 * cars in the right lane
 */

int goStraight(enum DIRECTION from, enum DIRECTION to){
	if (tellMeDirection[from] == 0){
		return (tellMeDirection[to] == 1) ? 1: 0;
	} else if(tellMeDirection[from] == 1){
		return (tellMeDirection[to] == 0) ? 1: 0;
	} else if(tellMeDirection[from] == 2){
		return (tellMeDirection[to] == 3) ? 1: 0;
	} else if(tellMeDirection[from] == 3){
		return (tellMeDirection[to] == 2) ? 1: 0;
	}
}

char* enToStr(enum DIRECTION f){
	char *x[4] = {"NORTH", "SOUTH", "EAST", "WEST"};

	return x[f];
}


char* lnToStr(enum LANE f){
	char *x[2] = {"LEFT","RIGHT"};

	return x[f];
}


////////////////////////////////////////////////////////////////


void* goingFromToP3(void *argu){
	struct argumentP3* args = (struct argumentP3*)(argu);
	int user_id = args->user_id;
	enum DIRECTION from = args->from;
	enum DIRECTION to = args->to;
	enum LANE lane = args->lane;


	
	int n = tellMeDirection[from];
	//printf("-> %s %s %s\n",enToStr(from),enToStr(to),lnToStr(lane) );
	if(lane == LEFT){
		pthread_mutex_lock(&signalData[n].mutexL);
		signalData[n].LeftWait++;            //lock
		pthread_mutex_unlock(&signalData[n].mutexL);
		sem_wait(&signalData[n].leftLane);
		if (!goStraight(from,to))
		{	
			signalData[n].whereToGo = 1;
			// signalData[tellMeDirection[from]].LeftWait--;         //lock
			printf("%s %s %s\n",enToStr(from),enToStr(to),lnToStr(lane) );fflush(stdout);

			return NULL;
		}
		signalData[n].whereToGo = 2;
		sem_wait(&signalData[tellMeDirection[from]].shouldSGo);

	}else {
		pthread_mutex_lock(&signalData[n].mutexL);
		signalData[n].RightWait++;           //lock
		pthread_mutex_unlock(&signalData[n].mutexL);
		sem_wait(&signalData[n].shouldRGo);
	}
	printf("%s %s %s\n",enToStr(from),enToStr(to),lnToStr(lane) );fflush(stdout);
	fflush(stdout);
}

void startP3(){}


void* SignalHandler(){
	TrafficLight = NORTH;
	int i =0;
	while(1){
		sem_wait(&signal);
		//i = (++i)%4;
		if (i == 0)
			i = 2;
		else if(i == 1)
			i = 3;
		else if(i == 2)
			i = 1;
		else
			i = 0;
		TrafficLight = tellMeDirection[i];
		sem_post(&signalAck);
	}

}


void* SideController(void* arg){
	int mySide = ((int*)(arg))[0];
	sleep(5);
	while(1){
		if (tellMeDirection[mySide] == TrafficLight)
		{
			//printf("TrafficLight %s\n", enToStr(TrafficLight));fflush(stdout);
			int n = 0;
			while(n < MAX_PASS && signalData[mySide].LeftWait)
			{
				if (!signalData[mySide].isInside)
					sem_post(&signalData[mySide].leftLane);
				
				pthread_mutex_lock(&signalData[mySide].mutexL);
				signalData[mySide].LeftWait--;
				pthread_mutex_unlock(&signalData[mySide].mutexL);
				
				while(!signalData[mySide].whereToGo){}
				if (signalData[mySide].whereToGo == 2)
				{
					n++;
					sem_post(&signalData[mySide].shouldSGo);
				}
				signalData[mySide].whereToGo = 0;
				signalData[mySide].isInside = 0;
			}
			while(n < MAX_PASS && signalData[mySide].RightWait){
				
				pthread_mutex_lock(&signalData[mySide].mutexL);
				signalData[mySide].RightWait--;
				pthread_mutex_unlock(&signalData[mySide].mutexL);
				
				sem_post(&signalData[mySide].shouldRGo);
				n++;
			}
			sleep(1);
			sem_post(&signal);
			sem_wait(&signalAck);
		}
		else
		{
			if (!signalData[mySide].isInside && signalData[mySide].LeftWait)
			{
				sem_post(&signalData[mySide].leftLane);
				while(!signalData[mySide].whereToGo){}
				if (signalData[mySide].whereToGo == 1)
				{
					pthread_mutex_lock(&signalData[mySide].mutexL);
					signalData[mySide].LeftWait--;
					pthread_mutex_unlock(&signalData[mySide].mutexL);
					signalData[mySide].whereToGo = 0;
				}
				else {
					signalData[mySide].isInside = 1;
				}
			}

		}
	}
}

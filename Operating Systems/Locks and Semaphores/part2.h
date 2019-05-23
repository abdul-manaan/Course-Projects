#ifndef PART_2
#define PART_2
struct trainArgs{
	int numStations;
	int maxNumPeople;
	int startPoint;
};
void initializeP2(int numStations, int maxNumPeople);
void *goingFromToP2(void *);
void startP2();

#endif

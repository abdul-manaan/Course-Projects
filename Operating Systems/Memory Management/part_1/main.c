#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define FRAME_SIZE 256
#define PHYSICAL_MEM 62

typedef struct logicalAddress{
	uint8_t pageNum;
	uint8_t Offset;
} logicalAddress_t;

typedef struct frame{
	char data[FRAME_SIZE];
} frame_t;

uint16_t pageTable[FRAME_SIZE];
frame_t  physicalMem[PHYSICAL_MEM];

logicalAddress_t* convert(int xx){
	logicalAddress_t *x = malloc(sizeof(logicalAddress_t));
	x->pageNum = (xx & 0x0000FF00) >> 8;
	x->Offset =  (xx & 0x000000FF);
	return x;
}

//size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
//int fseek(FILE *stream, long int offset, int whence)

void readFromBin(int offset, frame_t *frameToRead){
	FILE *bin;
	bin = fopen("BACKING_STORE.bin", "r");
	fseek(bin,offset, SEEK_SET);
	fread(frameToRead->data,FRAME_SIZE, 1, bin );
	fclose(bin);
	return;
}

uint8_t isFree[PHYSICAL_MEM];
uint8_t reference = 0;
uint8_t freeFrame(){
	//returns frame that is free
	reference = (reference+1)%FRAME_SIZE;
	for (uint8_t i = 0; i < PHYSICAL_MEM; ++i)
	{
		if (isFree[i])
		{
			isFree[i] = 0;
			return i;
		}
	}

	while(1) {
		uint8_t i = reference;
		do
		{
			if ((pageTable[i]&0x8000) != 0)
			{
				if (pageTable[i]&0x6000 == 0)
				{
					return pageTable[i]&0x00FF;
				}
			}
			i = (i+1)%FRAME_SIZE;
		}while(i != reference);

		uint8_t j = reference;
		do
		{
			if ((pageTable[j]&0x8000) != 0)
			{
				if (! (pageTable[j]&0x4000))
				{
					return pageTable[j]&0x00FF;
				}
				else {
					pageTable[j] = pageTable[j]&0xBFFF;
				}
			}
			j = (j+1)%FRAME_SIZE;
		}while(j != reference);
	}

}

void setPageTable(uint8_t frameNo){
	for (uint16_t i = 0; i < FRAME_SIZE; ++i)
	{
		if(pageTable[i] & 0x8000){
			if ((pageTable[i]&0x00FF) == frameNo)
			{
				pageTable[i] = 0;
				break;
			}
		}
	}
}
void pageFault(logicalAddress_t* logAdd){
	uint8_t frameNo = freeFrame();
	memset(&physicalMem[frameNo].data[0], 0, sizeof(FRAME_SIZE));
	readFromBin(logAdd->pageNum*FRAME_SIZE, &physicalMem[frameNo]);
	setPageTable(frameNo);
	pageTable[logAdd->pageNum] = 0x8000 | frameNo;
}


void readFromTable(logicalAddress_t* logAdd, int isWrite){
	char x[1];
	// printf("%d\n",(pageTable[logAdd->pageNum]&0x00FF) );
	char *y = physicalMem[ (pageTable[logAdd->pageNum]&0x00FF) ].data + logAdd->Offset;
	memcpy(x,y, 1 );
	// physicalMem[ pageTable[logAdd->pageNum]&0x00FF ] + logAdd->Offset;
	uint16_t physicalAddrress = ((pageTable[logAdd->pageNum]&0x00FF) << 8 )| logAdd->Offset;

	printf(" %4x %10x ", physicalAddrress, *x);


	pageTable[logAdd->pageNum] = pageTable[logAdd->pageNum] | 0x4000;

	if (isWrite)
	{
		pageTable[logAdd->pageNum] = pageTable[logAdd->pageNum] | 0x2000;
	}
}
int main(int argc, char *argv[]) {

   int num;
   int isWrite;

   for (int i = 0; i < PHYSICAL_MEM; ++i)
   {
   		isFree[i] = 1;
   }
   for (int i = 0; i < 256; ++i)
   {
   	   pageTable[i] = 0;
   }

   FILE *fp;
   fp = fopen(argv[1], "r"); // read mode
 
   if (fp == NULL)
   {
      perror("Error while opening the file.\n");
      exit(EXIT_FAILURE);
   }
   unsigned int numberOfPageFaults = 0;
   unsigned int totalReads = 0;

   printf("logicalAddress PhysicalAddress Value PageFault\n");
   while( fscanf(fp , "%x %d", &num, &isWrite) != EOF ){
   	  totalReads++;
      logicalAddress_t* logAdd = convert(num);
      int frameNum = pageTable[logAdd->pageNum];

      printf("%4x",num&0x0000FFFF);

      if (frameNum & 0x8000 ) { //No Page Fault
		readFromTable(logAdd,isWrite);
		printf(" No \n");

      } else {   // Page Fault
        numberOfPageFaults++;
        pageFault(logAdd);
        readFromTable(logAdd,isWrite);
		printf(" Yes \n");
      } 
      // printf("%d %d ", num, phyAdd);
   }
   
   printf("Page Faults (percantage):   =>  %f\n",100.0*numberOfPageFaults/totalReads );
   fclose(fp);
}
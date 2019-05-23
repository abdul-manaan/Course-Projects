#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define FRAME_SIZE 1024
#define PHYSICAL_MEM 95
#define LEVEL_1_BITS 6
#define LEVEL_2_BITS 8
#define OFFSET 10
#define LEVEL_2_ENTERIES 256
#define LEVEL_1_ENTERIES 64 
#define LEVEL_2_TABLES_IN_MEMORY 32
#define LEVEL_2_TABLES 64


typedef struct logicalAddress{
	uint16_t l_offset;
	uint8_t l_level2;
	uint8_t l_level1;
	uint16_t l_pageNum;
} logical_t;

logical_t* convert(int xx){
	logical_t *x = malloc(sizeof(logical_t));
	x->l_level1 = (xx & 0x00FC0000) >> 18;
	x->l_level2 = (xx & 0x0003FC00) >> 10;
	x->l_offset =  (xx & 0x000003FF);
	x->l_pageNum = (xx & 0x00FFFC00) >> 10;
	return x;
}

typedef struct frame{
	char data[FRAME_SIZE];
} frame_t;

typedef struct level2{
	uint32_t pageTable[LEVEL_2_ENTERIES];
} level2_t;

level2_t level2_pagetable[LEVEL_2_TABLES];
uint32_t level1_pagetable[LEVEL_1_ENTERIES];
frame_t  physicalMem[PHYSICAL_MEM];
int is_Level2_in_Memory[LEVEL_2_TABLES];
uint8_t isFree[PHYSICAL_MEM];

void readFromBin(int offset, frame_t *frameToRead){
	FILE *bin;
	bin = fopen("DATA_STORE.bin", "r");
	fseek(bin,offset, SEEK_SET);
	fread(frameToRead->data,FRAME_SIZE, 1, bin );
	fclose(bin);
	return;
}

uint16_t reference1 = 0;
uint16_t reference2 = 0;
uint8_t freeFrame(){
	//returns frame that is free
	if (reference1 == LEVEL_2_ENTERIES-1)
	{
		reference2 = (reference2+1)%LEVEL_2_TABLES;
	}
	reference1 = (reference1+1)%LEVEL_2_ENTERIES;
	for (uint8_t i = 0; i < PHYSICAL_MEM; ++i)
	{
		if (isFree[i])
		{
			isFree[i] = 0;
			return i;
		}
	}

	while(1){
		uint16_t i1 = reference1;
		uint16_t i2 = reference2;
		for (int i2 = 0; i2 < LEVEL_2_TABLES; ++i2)
			do
			{
				if ((level2_pagetable[i2].pageTable[i1]&0x80000000) != 0)
				{
					if ((level2_pagetable[i2].pageTable[i1]&0x60000000) == 0)
					{
						return level2_pagetable[i2].pageTable[i1]&0x0000FFFF;
					}
				}
				i1 = (i1+1)%LEVEL_2_ENTERIES;
			}while(i1 != reference1);

		uint16_t j1 = reference1;
		uint16_t j2 = reference2;
		for (int j2 = 0; j2 < LEVEL_2_TABLES; ++j2)
			do
			{
				if ((level2_pagetable[j2].pageTable[j1]&0x80000000) != 0)
				{
					if (! (level2_pagetable[j2].pageTable[j1]&0x40000000))
					{
						return level2_pagetable[j2].pageTable[j1]&0x0000FFFF;
					}
					else {
						level2_pagetable[j2].pageTable[j1] = level2_pagetable[j2].pageTable[j1]&0xBFFFFFFF;
					}
				}
				j1 = (j1+1)%LEVEL_2_ENTERIES;
			}while(j1 != reference1 );
	}

}

void setLevel2PageTable(uint8_t frameNo){
	for (int i = 0; i < LEVEL_2_TABLES; ++i)
	{
		for (int j = 0; j < LEVEL_2_ENTERIES; ++j)
		{
			if(level2_pagetable[i].pageTable[j] >> 31){
				if ((level2_pagetable[i].pageTable[j]&0x0000FFFF) == frameNo)
				{
					level2_pagetable[i].pageTable[j] = 0;
					return;
				}
			}
		}
	}
}

uint32_t getFreeLevel2Table(){
	for (int i = 0; i < LEVEL_2_TABLES; ++i)
	{
		if (is_Level2_in_Memory[i])
		{
			is_Level2_in_Memory[i] = 0;
			return i;
		}
	}
}

void pageFault(logical_t* logAdd){
	uint8_t frameNo = freeFrame();
	memset(&physicalMem[frameNo].data[0], 0, FRAME_SIZE);
	// printf("%d\n",logAdd->l_pageNum );
	readFromBin(logAdd->l_pageNum*FRAME_SIZE, &physicalMem[frameNo]);


	setLevel2PageTable(frameNo);
	uint32_t Level2PageNum = level1_pagetable[logAdd->l_level1];
	// level1_pagetable[logAdd->l_level1] = Level2PageNum;
	level2_pagetable[Level2PageNum].pageTable[logAdd->l_level2] = ((1<<31) | frameNo);
}


void readFromTable(logical_t* logAdd, int isWrite){
	char x[1];
	// printf("%d\n",(pageTable[logAdd->pageNum]&0x00FF) );
	char *y = physicalMem[ (level2_pagetable[level1_pagetable[logAdd->l_level1]].pageTable[logAdd->l_level2]&0x0000FFFF) ].data + logAdd->l_offset;
	memcpy(x,y, 1 );
	// physicalMem[ pageTable[logAdd->pageNum]&0x00FF ] + logAdd->Offset;
	uint16_t physicalAddrress = ((level2_pagetable[level1_pagetable[logAdd->l_level1]].pageTable[logAdd->l_level2]&0x0000FFFF) << 10 )| logAdd->l_offset;

	printf(" %4x %4x %10x ",(level2_pagetable[level1_pagetable[logAdd->l_level1]].pageTable[logAdd->l_level2]&0x0000FFFF), physicalAddrress, *x);


	level2_pagetable[level1_pagetable[logAdd->l_level1]].pageTable[logAdd->l_level2] = level2_pagetable[level1_pagetable[logAdd->l_level1]].pageTable[logAdd->l_level2] | 0x40000000;

	if (isWrite)
	{
		level2_pagetable[level1_pagetable[logAdd->l_level1]].pageTable[logAdd->l_level2] =level2_pagetable[level1_pagetable[logAdd->l_level1]].pageTable[logAdd->l_level2] | 0x20000000;
	}
}

uint16_t ref1 = 0;
int freeATable(){
	ref1 = (ref1 + 1)%LEVEL_2_TABLES;

	uint16_t i = ref1;
	do{
		if (is_Level2_in_Memory[i])
		{
			return i;
		}
		i = (i+1)%LEVEL_2_TABLES;
	}while(i != ref1);

}

void bringTableInMemory(int pa){
	uint16_t freeTable = freeATable();
	for (int i = 0; i < LEVEL_2_ENTERIES; ++i)
	{
		if (level2_pagetable[freeTable].pageTable[i]&0x80000000)
		{
			isFree[(level2_pagetable[freeTable].pageTable[i]&0x0000FFFF)] = 1;
			level2_pagetable[freeTable].pageTable[i] = 0;
		}
	}
	is_Level2_in_Memory[freeTable] = 0;
}
int main(int argc, char *argv[]) {

   int num;
   int isWrite;

   for (int i = 0; i < LEVEL_2_TABLES; ++i)
   {
   		if (i >= LEVEL_2_TABLES_IN_MEMORY)
   		{
   			is_Level2_in_Memory[i] = 0;
   		}else {
   			is_Level2_in_Memory[i] = 1;
   		}
   }
   for (int i = 0; i < LEVEL_1_ENTERIES; ++i)
   {
   	   	level1_pagetable[i] = i;
   }
   for (int i = 0; i < PHYSICAL_MEM; ++i)
   {
   		isFree[i] = 1;
   }

   FILE *fp;
   fp = fopen(argv[1], "r"); // read mode
 
   if (fp == NULL)
   {
      perror("Error while opening the file.\n");
      exit(EXIT_FAILURE);
   }
   double numberOfPageFaults = 0;
   double numberOfTableFaults = 0;
   double totalReads = 0;

   printf("logicalAddress PhysicalAddress Value PageFault\n");
   while( fscanf(fp , "%x %d", &num, &isWrite) != EOF ){
      totalReads++;
      logical_t* logAdd = convert(num);

      uint32_t Level2PageNum = level1_pagetable[logAdd->l_level1];

      uint32_t level2FrameNo = level2_pagetable[Level2PageNum].pageTable[logAdd->l_level2];
      printf("%8x ",num&0xFFFFFF );

      int isTableF = 0;
      if (!is_Level2_in_Memory[logAdd->l_level1])
      {
      	numberOfTableFaults++;
      	bringTableInMemory(logAdd->l_level1);
      	Level2PageNum = level1_pagetable[logAdd->l_level1];
      	level2FrameNo = level2_pagetable[Level2PageNum].pageTable[logAdd->l_level2];
      	is_Level2_in_Memory[logAdd->l_level1] = 1;
      	isTableF = 1;
      }

      if (( (level2FrameNo >> 31) )) { //No Page Fault
        readFromTable(logAdd,isWrite);
		if (isTableF)
		{
			printf(" Yes ");
		} else {
			printf("  No ");
		}
		printf("  No \n");

      } else {   // Page Fault
        pageFault(logAdd);
        readFromTable(logAdd,isWrite);
		if (isTableF)
		{
			printf(" Yes ");
		} else {
			printf("  No ");
		}
		printf(" Yes \n");
        numberOfPageFaults++;
      } 
   }
   printf("Statistics: \n");
   printf("\t Page Fault Rate => %2f\n" ,100.0*numberOfTableFaults/totalReads );
   printf("\t Frame Fault Rate => %2f\n" ,100.0*numberOfPageFaults/totalReads );
   // printf("count :   =>  %2f %2f\n",100.0*numberOfPageFaults/totalReads, 100.0*numberOfTableFaults/totalReads);
   fclose(fp);
}
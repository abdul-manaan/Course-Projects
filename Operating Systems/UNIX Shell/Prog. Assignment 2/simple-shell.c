#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include  <sys/types.h>
#include  <string.h>

#define MAX_LINE 80 /* The maximum length command */
#define BUF_SIZE 80 /* The maximum length command */

void parseArgum(char* args[], char arr[], int* isWait){
	int start = -1;
	int argNum = 0;
	int len = strlen(arr);
	int i;
	for ( i = 0; i <= len; ++i)
	{
		if ((arr[i] == ' ' || arr[i] == '\t') || arr[i] == '\n' || arr[i] == '\0')
		{
			if(start != -1){
				args[argNum] = &arr[start];
				argNum++;
			}
			start = -1;
			arr[i] = '\0';
		} else {
			if(start == -1){
				start = i;
			}
		}
	}
	//printf("%d\n",argNum);
	//fflush(stdout);
	if(args[argNum-1][0] == '&'){
		*isWait = 1;
		args[argNum-1] = NULL;
	} else {
		args[argNum] = NULL;
	}

}

struct command {
	int index;
	char com[MAX_LINE/2+1];
};


void addNewCommand(struct command comHist[], char com[], int* counter) {
	int i;	
	for (i = 9; i > 0; i--)
	{
		comHist[i] = comHist[i-1];
	}
	comHist[0].index = *counter;
	(*counter)++;
	strcpy(comHist[0].com, com);

}

void printHistory(struct command comHist[], int counter){
	int i;	
	if(counter){
		if(counter < 10){
			for (i = 0; i < counter; ++i)
			{
				printf("%d %s\n", comHist[i].index , comHist[i].com);
			}
		} else {
			for (i = 0; i < 10; ++i)
			{
				printf("%d %s\n", comHist[i].index , comHist[i].com);
			}
		}
	} else {
		printf("No Command in history\n");
	}
}
int main(void) { 
	char *args[MAX_LINE/2 + 1]; /* command line arguments */ 
	int should_run = 1; /* flag to determine when to exit program */ 
	pid_t pid;
	int status;
    char    buf[BUF_SIZE];
    int isWait = 0;

    struct command comHist[10];
    int counter = 0;

    char histCommand[] = "history";
    //comhist[0].index=counter

	while (should_run) { 
		isWait =0 ;
		printf("osh>");
		fflush(stdout);
		char argString[MAX_LINE/2+1];
		fgets(argString,MAX_LINE, stdin);


		char comTemp[MAX_LINE/2+1];
		strcpy(comTemp,argString);
		//addNewCommand(comHist, argString, &counter);
		parseArgum(args,argString,&isWait);
		//printf("%s\n",comHist[0].com);
		//fflush(stdout);

		if(strcmp(histCommand, args[0]) == 0){
			printHistory(comHist, counter);
			continue;
		} else if(strcmp("!!", args[0]) == 0) {
			if(counter){
				char newCom[MAX_LINE/2+1];
				strcpy(newCom,comHist[0].com);
				strcpy(comTemp,comHist[0].com);
				parseArgum(args,newCom,&isWait);

			} else {
				printf("No command in History\n");
				continue;
			}
		} else if(args[0][0] == '!'){
			args[0][0] = ' ';
			int x  = atoi(args[0]);	
			if(x >= 10 || (counter < 10 && x >= counter) || (x < 0)){
				printf("No Such Command in History\n");
				continue;
			} else {
				char newCom[MAX_LINE/2+1];
				strcpy(newCom,comHist[x].com);
				strcpy(comTemp,comHist[x].com);
				parseArgum(args,newCom,&isWait);				
			}
		}
		addNewCommand(comHist, comTemp, &counter);
        	if ((pid = fork()) < 0) {
  			printf("Failed to fork process 1\n");
			exit(1);
		}
	   	else if (pid == 0) {
			execvp(args[0], args);
		}
		if(isWait){
			while( pid != wait(&status));
		}
		/** * After reading user input, the steps are: * 
			(1) fork a child process using fork() * 
			(2) the child process will invoke execvp() * 
			(3) if command included &, parent will invoke wait() 
		**/
	}
	return 0;
}

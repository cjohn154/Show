#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include "semp.h"
#include <time.h>
#define numberOfSavages 10
#define M 10
using namespace std;

int servings;
Semaphore mutex, empty, full;
int savageNumber= 0;

void * cook(void * noparam)
{
	while(1)
	{
		int t = (int) rand() & 10;
		// wait for the pot to be empty
		semaphore_down(&empty);
		servings = M;
		sleep(t);
		printf("Cook has filled the pot\n");
		// pot is full, tell the savages
		semaphore_up(&full);
	}	
}

void * savage(void * noparam)
{	
	int eid,t;
	semaphore_down(&mutex);
	eid = savageNumber;
	savageNumber++;
	semaphore_up(&mutex);
	while(1)
	{
		t = (int) rand() & 10;
		semaphore_down(&mutex);
		if(servings ==0)
		{
			// tell the cook that the pot is empty
			printf("The pot is empty\n");
			semaphore_up(&empty);
			// wait for the pot to be full
			semaphore_down(&full);
		}
		servings--;
		semaphore_up(&mutex);
		printf("Savage %d: is eating\n",eid);
		sleep(t);
	}
}

main() 
{
  	servings = M;
  	long cookCount = 1;
 	pthread_t savages[numberOfSavages];
  	pthread_t theCook;
	 //init semaphore multex to 1, so first person can entry critical section  
  	semaphore_init(&mutex,1);
  	semaphore_init(&full,0);
  	semaphore_init(&empty,0);
	// Creating the Savage thread
  	for (int i=0; i<numberOfSavages; i++)
	{
   	   // NOTES: Possible error in the create method with the i, check if things are not working
    	 	if (pthread_create(&savages[i], NULL, savage, &i)) 
		{
      	 		perror("error creating the thread");
       			exit(1);
     		}
  	}
	printf ("Savage threads created \n");
	// Creating the Cook thread
   	if (pthread_create(&theCook, NULL, cook,(void*)cookCount )) 
   	{
      		 perror("error creating the thread");
      		 exit(1);
     	}
	printf("Cook thread created \n");
  	//wait for threads to finish 
  	for(int i=0;i<numberOfSavages;i++)
  	{
   		 pthread_join(savages[i], 0);
 	}
  	pthread_join(theCook,0);

}

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include "semp.h"
#include <time.h>
#define numberOfPhil 5
using namespace std;
Semaphore chopsticks[numberOfPhil];
Semaphore mutex;
bool isEating[numberOfPhil]; 
int philosopherNumber = 0; 

void * philosopher(void * noparam) 
{
  int eid, t,tempEid,nextLeft, nextRight;
  semaphore_down(&mutex);
  eid = philosopherNumber;
  philosopherNumber++;
  semaphore_up(&mutex);
  // if a philospher is not eating, then it is thinking
  // Checking the left and right of the philsophers so if we are at the fifth philopher, his right is the first
  int left; 
  int right;
  left = (eid+4)%numberOfPhil;
  right = (eid+1)%numberOfPhil;
  while (1==1)  
  {
    t = (int) rand() % 10; 
    printf("philosopher %d: wants to eat\n",eid);
    // Taking fork
    semaphore_down(&mutex);
    if ((isEating[eid] == false) && (isEating[left] != true) && (isEating[right] != true)) {  // checking if is not currently eating and if left and right are not eating
        // philosopher is eating 
	printf("philosopher %d: is to eating\n",eid);
        isEating[eid] = true;
	semaphore_up(&mutex);
        sleep(t);
	semaphore_up(&chopsticks[eid]);  
    } 
    else
    {
      semaphore_up(&mutex);
    }
    semaphore_down(&chopsticks[eid]);
    sleep(t);
    // Putting chopsticks down
    semaphore_down(&mutex);
    // Philsopher is longer eating, now they are thinking
    isEating[eid] = false;
    semaphore_up(&mutex);
    printf("philosopher %d: is thinking for %d seconds\n",eid,t);
    sleep(t);
    // must check if we can test the chopstick to prevent deadlock
    // must check the status of the left
    tempEid = left;
    nextLeft = (tempEid+4)%numberOfPhil;
    nextRight = (tempEid+1)%numberOfPhil;
    semaphore_down(&mutex);
    if ((isEating[tempEid] == false) && (isEating[nextLeft] != true) && (isEating[nextRight] != true)) {  // checking if is not currently eating and if left and right are not eating
       // philsopher is eating now
      isEating[tempEid] = true;
      semaphore_up(&mutex);
      sleep(t);
      semaphore_up(&chopsticks[tempEid]);  
    }
    else
    {
      semaphore_up(&mutex);
    }
    // must check the status of the right
    tempEid = right;
    nextLeft = (tempEid+4)%numberOfPhil;
    nextRight = (tempEid+1)%numberOfPhil;
    semaphore_down(&mutex);
    if ((isEating[tempEid] == false) && (isEating[nextLeft] != true) && (isEating[nextRight] != true)) 
    {  
      // checking if is not currently eating and if left and right are not eating
      // philsopher is eating now
      isEating[tempEid] = true;
      semaphore_up(&mutex);
      sleep(t);
      semaphore_up(&chopsticks[tempEid]);  
    }
    else
    {   
      semaphore_up(&mutex);
    }
  }
}

main() 
{
  pthread_t philosophers[numberOfPhil]; 
  srand((unsigned)time(0));
  for(int i =0; i <numberOfPhil;i++)
  {
      // initializing the philosophers to not eating
      isEating[i] = false;
  }
  // init semaphore line to 0, since everyone is not eating to begin with
  for(int i = 0; i < numberOfPhil; i++)
  {
    semaphore_init(&chopsticks[i],0);
  }
  // init semaphore multex to 1, so first person can entry critical section  
  semaphore_init(&mutex,1);
  for (int i=0; i<numberOfPhil; i++) 
  {
     if (pthread_create(&philosophers[i], 0,philosopher, &i)) {
       perror("error creating the thread");
       exit(1);
     }
  }
  printf("philosophers threads created \n"); 
  // wait for threads to finish 
  for(int i=0;i<numberOfPhil;i++)
  {
    pthread_join(philosophers[i], 0);
  }
}

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include "semp.h"
#include <time.h>
#define numberOfPhysicalPlant 5
#define numberOfClient 2
using namespace std;

Semaphore theHelpDesk;
Semaphore theClient;
Semaphore thePhysicalPlant;
Semaphore mutex;
int ready = 0;
int techNumber = 0;
int clientNumber = 0;
int availableTech = 0;

void * helpDesk(void * noparam) 
{
  while(1)
  {
    // waiting on calls from clients
    semaphore_down(&theHelpDesk);
    printf("Sent techs to fix problem \n");
    // recieved calls from clients, sending the Physical Plant Techs to fix the problems 
    semaphore_up(&thePhysicalPlant);
  }
}

void * physicalPlant(void * noparam) 
{
  int eid, t;
  semaphore_down(&mutex);
  eid = techNumber;
  techNumber++;
  semaphore_up(&mutex);
  printf("tech %d: is starting \n",eid);
  while(1)
  {
    t = (int) rand() % 10; 
    printf("tech %d: is drinking coffee for %d seconds\n",eid,t);
    sleep (t);
    semaphore_down(&mutex);
    availableTech++;
    if(availableTech==3)
    {
      // There are three available techs
      availableTech -=3;
      semaphore_up(&mutex);
      semaphore_down(&thePhysicalPlant);
      semaphore_up(&thePhysicalPlant);
      semaphore_up(&thePhysicalPlant);
    }
    else
    {
      // There is not three available techs
      semaphore_up(&mutex);
      semaphore_down(&thePhysicalPlant);
    }
    //tech is helping
    printf("techs are coming to help\n");
    // communicating with the client that techs are coming to help
    semaphore_up(&theClient);
  }
}

void * client(void * noparam) 
{
  int eid, t;
  semaphore_down(&mutex);
  eid = clientNumber;
  clientNumber++ ;
  semaphore_up(&mutex);
  printf("client %d: is starting \n",eid);
  while(1)
  {
    t = (int) rand() % 10; 
    printf("client %d: is working %d seconds\n",eid,t);
    sleep (t);
    // Tell the help desk that the client need help
    semaphore_up(&theHelpDesk);
    printf("client %d: asked for help\n",eid);
    // Wait to head back from the help desk that techs are coming to help
    semaphore_down(&theClient);
  }
}

main()
{
  pthread_t helpDesks;
  pthread_t clients[numberOfClient];
  pthread_t physicalPlants[numberOfPhysicalPlant];
  srand((unsigned)time(0));
  // initialize semaphores
  semaphore_init(&theClient,0);
  semaphore_init(&thePhysicalPlant,0);
  semaphore_init(&mutex,1);
  semaphore_init(&theHelpDesk,0);
  for (int i=0; i<numberOfClient; i++) 
  {
     // Creating the client threads
     if (pthread_create(&clients[i], 0,client, 0)) 
     {
       perror("error creating the thread");
       exit(1);
     }
  }
  for (int i=0; i<numberOfPhysicalPlant; i++) 
  {
    // Creating the physical plants threads for physical plant techs 
    if (pthread_create(&physicalPlants[i], 0,physicalPlant, 0)) 
    {
      perror("error creating the thread");
      exit(1);
    }
  }
   // Creating the help desk thread
   if (pthread_create(&helpDesks,0,helpDesk, 0)) 
   {
      perror("error creating the thread");
      exit(1);
    }
  printf ("Physical Plant threads created \n");
  // wait for the physical plant threads to finish 
  for(int i=0;i<numberOfPhysicalPlant;i++)
  {
    pthread_join(physicalPlants[i], 0);
  }
  
  printf ("Client threads created \n");
  // wait for client threads to finish threads to finish 
  for(int i=0;i<numberOfClient;i++)
  {
    pthread_join(clients[i], 0);
  }
  // wait for the help desk thread to finish
  pthread_join(helpDesks,0);
}

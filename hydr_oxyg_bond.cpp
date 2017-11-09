#include<iostream>
#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
using namespace std;

pthread_t thread_oxy;		//Thread for oxygen
pthread_t thread_hydro;		//Thread for oxygen
pthread_mutex_t lock;   		//Lock for oxy
pthread_mutex_t lock1;
pthread_cond_t oxy_cond;;
pthread_cond_t hydro_cond;;
void *status;
sem_t o; 			//Semaphore for oxy.
sem_t h;			//Semaphore for oxy.
int oxy,hydro;			//variables to get Count of oxygen and hydrogen from semaphore.
int count=0;

void *oxygen(void *);		//Oxygen thread.
void *hydrogen(void *);		//Hydrogen thread.

void bond()
{
   cout<<"Bond is created "<<count<<endl;
   count++;					//To keep count of number of bond created.
}

int main()
{
   sem_init(&o,0,0);				//Initialize semaphore for oxygen.
   sem_init(&h,0,0);				//Initialize semaphore for hydrogen.
   for(int k=0;k<10;k++)			//Call Oxygen thread 10 times.
   {
   if(pthread_create(&thread_oxy, NULL, oxygen,NULL))	//Create Oxygen thread.
	cout<<"Error while creating thread"<<endl;
   }
   for(int k=0;k<20;k++)				//Create Hydrogen 20 times.
   {
   if(pthread_create(&thread_hydro, NULL, hydrogen,NULL))	//Create Hydrogen thread.
	cout<<"Error while creating thread"<<endl;
   }
   pthread_join(thread_oxy,&status);			//Join Oxygen thread.
   pthread_join(thread_hydro,&status);			//Join Hydrogen thread.
   return 0;
}

void *oxygen(void *i)
{
   pthread_mutex_lock(&lock);				//Acquire Lock.
   sem_post(&o);
   if(sem_getvalue(&o, &oxy))				//Get value of oxygen semaphore.
      cout<<"Error in getting value";
   if(sem_getvalue(&h, &hydro))				//Get value of hydrogen semaphore.
      cout<<"Error in getting value";
   if(oxy>=1 && hydro>=2)				//Check for bond condition.
   {
      bond();
      sem_wait(&h);					//Decrease Hydrogen count by 2.
      sem_wait(&h);
      sem_wait(&o);					//Decrease Oxygen count by. 
      pthread_cond_signal(&oxy_cond);			//Wake up one oxygen thread.
      pthread_cond_signal(&hydro_cond);			//Wake up two hydrogen thread.
      pthread_cond_signal(&hydro_cond);
   }
   else
   {
      pthread_cond_wait(&oxy_cond,&lock);		//Wait for bond to be formed.
   }
   pthread_mutex_unlock(&lock);				//Unlock Mutex.
   pthread_exit(0);
}

void *hydrogen(void *i)
{
   pthread_mutex_lock(&lock);
   sem_post(&h);
   if(sem_getvalue(&o, &oxy))
      cout<<"Error in getting value";
   if(sem_getvalue(&h, &hydro))
      cout<<"Error in getting value";
   if(oxy>=1 && hydro>=2)
   {
      bond();
      sem_wait(&h);
      sem_wait(&h);
      sem_wait(&o);
      pthread_cond_signal(&oxy_cond);
      pthread_cond_signal(&hydro_cond);
      pthread_cond_signal(&hydro_cond);
   }
   else
   {
      pthread_cond_wait(&hydro_cond,&lock);
   }
   pthread_mutex_unlock(&lock);
   pthread_exit(0);
}

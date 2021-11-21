#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "buffer.h"

pthread_mutex_t mutex;
pthread_attr_t attr;
sem_t full, empty;
buffer_item buffer[BUFFER_SIZE];
int count, totalItems, itemCount, itemsProduced, itemsProcessed, in, out;

void *producer(void *param);
void *consumer(void *param); 

int insert_item(buffer_item item) {
 if(itemsProduced<BUFFER_SIZE && totalItems<itemCount){
  buffer[in] = item;
  in = (in+1)%BUFFER_SIZE;
  return 0;
 } else{
  printf("Unable to insert\n"); 
  return -1;
 }
}

int remove_item(buffer_item *item) {
 if(itemsProduced>0 && itemsProcessed<itemCount){
  *item = buffer[out];
  out = (out+1)%BUFFER_SIZE;
  return 0;
 } else{
  printf("Unable to remove\n");
  return -1;
 }
}

void *producer(void *param){
 buffer_item item;
 while(itemsProcessed<itemCount){
  if(totalItems>=itemCount) break;
  int time = rand()/100000000;
  usleep(time);
  item = rand()/1000000;
  
  sem_wait(&empty);
  pthread_mutex_lock(&mutex);
  if(totalItems<itemCount){
   if(insert_item(item)){
    fprintf(stderr, "production failed\n"); 
   }else{
    printf("producer %d produced %d\n",param, item);
    itemsProduced++;
    totalItems++;
   }
  }

  pthread_mutex_unlock(&mutex);
  sem_post(&full);
 }
 pthread_exit(NULL);
}

void *consumer(void *param) {
 buffer_item item;
 while(itemsProcessed<itemCount){
  if(itemsProcessed>=itemCount) break;
  int time = rand()/100000000;
  usleep(time);
  
  sem_wait(&full);
  pthread_mutex_lock(&mutex);
  
  if(itemsProcessed<itemCount){
   if(itemsProduced>0 && remove_item(&item)){
    fprintf(stderr, "consumption failed\n");
   }
   else{
    printf("consumer %d consumed %d\n", param, item);
    itemsProcessed++;
    itemsProduced--;
   }
  } 
  pthread_mutex_unlock(&mutex);
  sem_post(&empty);
 }
 pthread_exit(NULL);
}

void main(int argc, char *argv[]){
 int numOfProducers, numOfConsumers;
 if(argc<3) { 
  fprintf(stderr, "Not enough arguments.\nUsage: ./procSync [numOfProducers(int)] [numOfConsumers(int)] [numOfItems(int)]\n");
  exit(0);
 }
 else {
  numOfProducers = atoi(argv[1]);
  numOfConsumers = atoi(argv[2]);
  itemCount = atoi(argv[3]);
  in = 0;
  out = 0;
 }

 pthread_mutex_init(&mutex, NULL);
 sem_init(&full, 0, 0);
 sem_init(&empty, 0, BUFFER_SIZE);
 pthread_attr_init(&attr);
 itemsProduced=0;
 itemsProcessed=0;
 totalItems=0;
 pthread_t protid[numOfProducers], contid[numOfConsumers];

 int i;
 for(i=0; i<numOfProducers; i++){ 
  pthread_create(&protid[i], &attr, (void *)producer, (void *)i);
 }
 for(i=0; i<numOfConsumers; i++){
  pthread_create(&contid[i], &attr, (void *)consumer, (void *)i);
 }
 for(i=0; i<numOfProducers; i++){
  pthread_join(protid[i], NULL);
 }
 for(i=0; i<numOfConsumers; i++){
  pthread_join(contid[i], NULL);
 }
 
 pthread_mutex_destroy(&mutex);
 sem_destroy(&empty);
 sem_destroy(&full);
 printf("exiting\n");
 exit(0);
 
}

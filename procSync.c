#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "buffer.h"

pthread_mutex_t mutex;
pthread_attr_t attr;
sem_t full, empty;
buffer_item buffer[BUFFER_SIZE];
int itemCount, itemsProduced, itemsProcessed, in, out;

void *producer(void *param);
void *consumer(void *param); 

int insert_item(buffer_item item) {
 //if(itemCount>=BUFFER_SIZE) return -1;
 //else {
 // buffer[itemCount++] = item;
 // return 0;
 //}
 if(itemsProduced!=BUFFER_SIZE){
  buffer[in] = item;
  in = (in+1)%BUFFER_SIZE;
  itemsProduced++;
  return 0;
 } else{
  printf("Unable to insert\n"); 
 }
}

int remove_item(buffer_item *item) {
 //if(itemCount<=0 || item==NULL) return -1;
 //else {
 // *item = buffer[itemCount-1];
 // itemCount--;
 //}
 if(itemsProcessed<=itemsProduced){
  *item = buffer[out];
  out = (out+1)%BUFFER_SIZE;
  itemsProcessed--;
  return 0;
 } else{
  printf("Unable to remove");
  return -1;
 }
}

void *producer(void *param){
 buffer_item item;
 //printf("producer\n");
 while(itemsProduced<=itemCount){
 //while(1){
  //if(itemCount<=itemsProduced) break;
  int time = rand()/100000000;
  usleep(time);
  item = rand()/100000000;
  
  sem_wait(&empty);
  pthread_mutex_lock(&mutex);
 
  if(insert_item(item)) fprintf(stderr, "production failed\n");
  else printf("producer %d produced %d\n",param, item);
  
  itemsProduced++;
  pthread_mutex_unlock(&mutex);
  sem_post(&full);
 }
 pthread_exit(NULL);
}

void *consumer(void *param) {
 buffer_item item;
 //printf("consumer\n");
 while(itemsProcessed<=itemCount){
 //while(1){
   int time = rand()/100000000;
  usleep(time);
  
  sem_wait(&full);
  pthread_mutex_lock(&mutex);

  if(remove_item(&item)) fprintf(stderr, "consumption failed\n");
  else printf("consumer %d consumed %d\n", param, item);

  //itemsProcessed++; 
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
 //printf("%d %d %d \n",numOfProducers, numOfConsumers, numOfItems);
 //exit(0);
 pthread_mutex_init(&mutex, NULL);
 sem_init(&full, 0, 0);
 sem_init(&empty, 0, BUFFER_SIZE);
 pthread_attr_init(&attr);
 itemsProduced=0;
 itemsProcessed=0;
 pthread_t protid[numOfProducers], contid[numOfConsumers];

 int i;
 for(i=0; i<numOfProducers; i++){ 
  //printf("%d\n",i);
  //pthread_attr_init(&attr);
  //char id[16];
  //sprintf(id, "%d", i);
  pthread_create(&protid[i], &attr, (void *)producer, (void *)i);
 }
 for(i=0; i<numOfConsumers; i++){
  //printf("%d\n",i);
  //pthread_attr_init(&attr);
  //char id[16];
  //sprintf(id, "%d", i);
  //printf("%s\n",id);
  pthread_create(&contid[i], &attr, (void *)consumer, (void *)i);
 }
 //pthread_mutex_destroy(&mutex);
 //sem_destroy(&empty);
 //sem_destroy(&full);
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
 //pthread_mutex_destroy(&mutex);
 //sem_destroy(&empty);
 //sem_destroy(&full);
 //if(itemsProduced == itemsProcessed) { 
 // printf("exiting\n");
 // exit(0);
 // pthread_mutex_destroy(&mutex);
 // sem_destroy(&empty);
 // sem_destroy(&full);
 //} 
}

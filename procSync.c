#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "buffer.h"

pthread_mutex_t mutex;
pthread_attr_t attr;
pthread_t tid;
sem_t full, empty;
buffer_item buffer[BUFFER_SIZE];
int itemCount;


int insert_item(buffer_item item) {
 if(itemCount>=BUFFER_SIZE) return -1;
 else {
  buffer[itemCount++] = item;
  return 0;
 }
}

int remove_item(buffer_item *item) {
 if(itemCount<=0 || item==NULL) return -1;
 else {
  *item = buffer[itemCount-1];
  itemCount--;
 }
}

void *producer(void *param){
 buffer_item item;
 while(0){
  int time = rand();
  usleep(time);
  item = rand();
  
  sem_wait(&empty);
  pthread_mutex_lock(&mutex);
 
  if(insert_item(item)) fprintf(stderr, "report error condition");
  else printf("producer produced %d\n",item);
  
  pthread_mutex_unlock(&mutex);
  sem_post(&full);
 }
}

void *consumer(void *param) {
 buffer_item item;
 while(0){
  int time = rand();
  usleep(time);
  
  sem_wait(&full);
  pthread_mutex_lock(&mutex);

  if(remove_item(&item)) fprintf(stderr, "report error condition");
  else printf("consumer consumed %d\n", item);
  
  pthread_mutex_unlock(&mutex);
  sem_post(&empty);
 }
}

void main(int argc, char *argv[]){
 int numOfProducers, numOfConsumers, numOfItems;
 if(argc<3) { 
  fprintf(stderr, "Not enough arguments.\nUsage: ./procSync [numOfProducers(int)] [numOfConsumers(int)] [numOfItems(int)]\n");
  exit(0);
 }
 else {
  numOfProducers = atoi(argv[1]);
  numOfConsumers = atoi(argv[2]);
  numOfItems = atoi(argv[3]);
 }
 //printf("%d %d %d \n",numOfProducers, numOfConsumers, numOfItems);
 //exit(0);
 pthread_mutex_init(&mutex, NULL);
 sem_init(&full, 0, 0);
 sem_init(&empty, 0, BUFFER_SIZE);
 pthread_attr_init(&attr);
 itemCount=0;
 
 int i;
 for(i=0; i<numOfProducers; i++){ 
  //printf("%d\n",i);
  pthread_create(&tid, &attr, producer, NULL);
 }
 for(i=0; i<numOfConsumers; i++){
  //printf("%d\n",i);
  pthread_create(&tid, &attr, consumer, NULL);
 }
 //pthread_mutex_destroy(&mutex);
 //sem_destroy(&empty);
 //sem_destroy(&full);
 
 printf("Exit the program\n");
 exit(0); 
}

//
//  pid-producer-consumer.c
//  This program is to demonstrate the producer consumer problem
//  The Producer fills out the a PID(Persistent Identifier)-Queue
//  A PID is a alphanumeric string
//  The Consumers take PIDs out of the PID-Queue
//  In order to guarantee a proper functionality, a synchronization mechanisim
//  must be established.
//  This synchronization mechanism is based on condition variables



//  Created by Fatih Berber on 25.05.15.
//  Copyright (c) 2015 GWDG. All rights reserved.
//

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#define QUEUE_MAX           1000
#define CONSUMER_THREADS    10
#define PID_COUNT           500
#define PID_PREFIX_LEN      6
#define PID_SUFFIX_LEN      14
#define PID_LEN             (PID_PREFIX_LEN + PID_SUFFIX_LEN)

/*---------------------------------------------------*/
int producer_keep_running = 1;
/*---------------------------------------------------*/
/*-------declare Producer-Consumer variables here-----*/
/*---------------------------------------------------*/

pthread_mutex_t mutex;
pthread_mutex_t mutexp;
pthread_mutex_t mutexc;
pthread_cond_t consume;
pthread_cond_t produce;



/*---------------------------------------------------*/
/*---------------------------------------------------*/
/*---------------------------------------------------*/

struct pid_node{
    char* pid_str;
    struct pid_node *next_node;
};
typedef struct pid_node* pid_node;

struct the_queue{
    
    pid_node head_node;
    int queue_current_size;
    int queue_max_size;
    unsigned int filled;
    
} pid_queue;



void insertPID(char* pid){
    
    pid_node new_p_node = (pid_node) malloc(sizeof(struct pid_node));
    new_p_node->pid_str = pid;
    new_p_node->next_node = pid_queue.head_node;
    pid_queue.head_node  = new_p_node;
    pid_queue.queue_current_size++;
    if (pid_queue.queue_current_size == pid_queue.queue_max_size)
        pid_queue.filled = 1;
    else
        pid_queue.filled = 0;

}

char* removePID(){
    
    pid_node pnode = pid_queue.head_node;
    pid_queue.head_node = pnode->next_node;
    pid_queue.queue_current_size--;
    if (pid_queue.queue_current_size == 0)
        pid_queue.filled = 0;
    
    return pnode->pid_str;
    
}


void queue_filler(){
    
    char* pid;
    const char *prefix = "12345/";
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVXYZ1234567890";
    int i, key;
    while(!pid_queue.filled){
        
        pid = (char*) malloc((PID_LEN+1)*sizeof(char));
        
        strncpy(pid,prefix,PID_PREFIX_LEN);
        // generate an alphanumeric string representing a pid
        for (i = PID_PREFIX_LEN; i < PID_LEN; i++) {
            key = rand() % (int) (sizeof charset - 1);
            if (i == 10  || i == 15)
                pid[i] = '-';
            else
                pid[i] = charset[key];
        }
        pid[PID_LEN] = '\0';
        // put the random pid to the queue
        insertPID(pid);
    }
   
  printf("Producer is done with filling the queue\n"); 
    
}




char* getPID(){
    
    
    char *pid;
    
    /*++++++++++++ add the missing code snippets here ++++++++++*/
    pthread_mutex_lock(&mutexc);
    int wait = 1;
    while(wait){
        pthread_cond_wait(&consume,&mutexc);
        pthread_mutex_unlock(&mutexc);
        pid = removePID();
        pthread_mutex_lock(&mutexc);
        wait = 0;
    }
    /*++++++++++++ add the missing code snippets here ++++++++++*/
    if(!pid_queue.filled){
        pthread_cond_signal(&produce);
    }
    pthread_mutex_unlock(&mutexc);
    
    return pid;
    
}



void putPID(){
    
    pthread_mutex_lock(&mutexp);
    while(producer_keep_running)
    {
        
        /*++++++++++++ add the missing code snippets here ++++++++++*/
        if(pid_queue.filled){
            pthread_cond_wait(&produce,&mutexp);
        }
        
        pthread_mutex_unlock(&mutexp);
        queue_filler();
        pthread_mutex_lock(&mutexp);
        
        
        /*++++++++++++ add the missing code snippets here ++++++++++*/
        pthread_cond_broadcast(&consume);
        
    }
    pthread_mutex_unlock(&mutexp);
    
    
}



void* pid_consumer(void* cid){
    
    int i;
    int pid_element;
    long c_id = (long)cid;
    char *pid_list[PID_COUNT];
    
    printf("consumer[%ld] started to ask for PIDs\n",c_id);
    for(i = 0; i < PID_COUNT;i++)
        pid_list[i] = getPID();
    
    pid_element = rand() % (int) (PID_COUNT - 1);
    printf("cosumer[%ld] got his %d PIDs, PID[%d] = %s \n",c_id,PID_COUNT,pid_element,pid_list[pid_element]);
    pthread_exit(NULL);
    
}



void* pid_producer(){
    
    putPID();
    pthread_exit(NULL);

}


void init_pid_queue(){
    
    pid_queue.filled = 0;
    pid_queue.queue_max_size = QUEUE_MAX;
    pid_queue.queue_current_size = 0;

}

void init_pthread_variables(){
    
 
    /*++++++++++++ init the previously declared pthread variables here ++++++++++*/
    pthread_cond_init(&consume,NULL);
    pthread_cond_init(&produce,NULL);
    pthread_mutex_init(&mutex,NULL);


}


int main(int argc, const char * argv[]) {

    long i;
    
    pthread_t consumer_threads[CONSUMER_THREADS];
    pthread_t producer_thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    
    init_pthread_variables();
    init_pid_queue();
    
    for (i = 0;i < CONSUMER_THREADS;i++){
      pthread_create(&consumer_threads[i], &attr, pid_consumer, (void *)i);
    
    }
    
    pthread_create(&producer_thread, &attr, pid_producer,NULL);
    
    for(i=0; i<CONSUMER_THREADS; i++)
        pthread_join(consumer_threads[i], NULL);
    
    producer_keep_running = 0;
    
    
    return 0;
}

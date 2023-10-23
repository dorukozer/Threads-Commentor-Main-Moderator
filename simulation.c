#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
/**
 * pthread_sleep takes an integer number of seconds to pause the current thread
 * original by Yingwu Zhu
 * updated by Muhammed Nufail Farooqi
 * updated by Fahrican Kosar
 */
// unsigned int* seed;





//these are all global variables that i used for the comminication of threads
int  finish_break=0;
int counter_b;
double ** hourss;
 int ** state;
 int* Breaking_News;
int mic_global;
int boo4=0;
int boo3 = 0; 
 int seed=0;
int turn=0; 
int boo=0;
int boo2=0;
int control_bit=0;
int request_Queue[16] ;//global request queue
int question_number=10 ;//number of questions
pthread_t comment[16];//thread of commentator
pthread_t moderator;

pthread_t main_thread;

int b_signalled=0;

pthread_t moderator;//moderator thread
void *commentator_void(void *input ); 
int queue_count; // to keep track of queue
void * moderator_void ( void* global_queue  );
int  possible_question( int id,int index );
int random_number= 1;
int flg =0;

int possible_question();

pthread_mutex_t mutex0;
pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
pthread_mutex_t mutex3;
pthread_mutex_t mutex4;
pthread_mutex_t mutex6;
pthread_mutex_t breaking;

pthread_mutex_t mutexforwait;


pthread_mutex_t mutex5;
pthread_mutex_t question_time; //to check all commentator asked questions
int pthread_sleep(double seconds);

pthread_barrier_t barrier0;
pthread_barrier_t barrier; // to check if the all commentator put their questions on the queue
pthread_cond_t condition0;
pthread_cond_t condition1;
pthread_cond_t bcond;
pthread_cond_t bcond2;
pthread_cond_t wake_up;
pthread_cond_t question_asked; //commentator waits on this and checks if the question is asked
pthread_cond_t all_enqueued; // this condition is to check wether all questions are enqueued
int queue[16];

void * breaking_news ();

typedef struct Queue  {// queue structure with mutex
int counter;  //keep track of queue 
int *queue;

}Queue;



struct args {
    int id;
    Queue* q;
    
};


int counter;  //keep track of queue 
int cou;

void enqueue(Queue *q ,int id);
int global_thread_number;
double  global_possibility;
double  global_tspeak ;
int  global_question ;
int global_break;
int main(int argc, char** argv){





 for (int i = 1; i < argc; i++){
        if (strcmp(argv[i],"-n")==0) {
                global_thread_number=  atoi(argv[i+1]);
               
             }
             if (strcmp(argv[i],"-q")==0) {
                 global_question  = atoi(argv[i+1]);
             }
             if (strcmp(argv[i],"-p")==0)
             {
                global_possibility  = 100*(atof(argv[i+1]));
            
             }
             if (strcmp(argv[i],"-t")==0)
             {
                 global_tspeak  = atof(argv[i+1]);
             }
             if (strcmp(argv[i],"-b")==0)
             {
                 global_break  = 100*atof(argv[i+1]);
             }           
             }










//Since random numbers are not thread unique we generated them in the program start
//All generated random numbers stored in specific global array 




//this state 2D array is for the possibility of question
//we stored the numbers global question possibility number which is determined by -p flag with multiplying with 100
//then the way we store by taking the remainder of 100 is to determine is it smaller than global possibility number
//we solved the possibility cases for breaking news event and possibility of question in this manner.

srand(time(NULL));
state= malloc(sizeof(int*)*global_thread_number);// 16 is number of threads

for(int i = 0 ; i<global_thread_number;i++ ){
state[i]= malloc(sizeof(int)*global_question);//15 is number of question
}


for(int i = 0 ; i < global_thread_number;i++){
for(int j = 0 ; j < global_question;j++){
  *(*(state+i)+j)= rand() % 100 + 1;
}
}











//I stored random seconds which are for threads who want to ask in hourss 2D array so  
hourss = malloc(sizeof(double*)*global_thread_number);

for(int i = 0 ; i<global_thread_number;i++ ){
hourss[i]= malloc(sizeof(double)*global_question);
}


for(int i = 0 ; i < global_thread_number;i++){
for(int j = 0 ; j < global_question;j++){
  *(*(hourss+i)+j)= ( global_tspeak / RAND_MAX) * rand();
 
}
}

//this is also pre recorded breaking news possibility numbers
//it is the same logic in the possibility question
Breaking_News = malloc(sizeof(int)*global_thread_number*global_question);
for(int i = 0 ; i< global_thread_number*global_question ;i++ ){
Breaking_News[i]= rand() % 100 + 1;
}





//this is the global queue
Queue que; 
que.counter=0;
que.queue=malloc(global_thread_number*sizeof(int));
 
 counter=0;
 cou=0;


//initilization of all mutexes
//--------------------------------------------------------------------------
 pthread_mutex_init(&( mutex0),NULL);
  pthread_mutex_init(&(breaking),NULL);
 pthread_cond_init (&( condition0), NULL);
  pthread_cond_init (&( bcond), NULL);
   pthread_cond_init (&( bcond2), NULL);
 pthread_cond_init (&( condition1), NULL);
 pthread_mutex_init(&( mutex1),NULL);
 pthread_mutex_init(&( mutex2),NULL);
 pthread_cond_init(&( question_asked), NULL);
 pthread_cond_init (&( all_enqueued), NULL);
 pthread_mutex_init(&(mutex3),NULL);
 pthread_mutex_init(&(mutex5),NULL);
  pthread_mutex_init(&(mutex6),NULL);
 pthread_cond_init (&(wake_up),NULL);
 pthread_mutex_init(&( mutex4),NULL);
  pthread_mutex_init(&( mutexforwait),NULL);
 pthread_barrier_init(&( barrier),NULL,global_thread_number);
  pthread_barrier_init(&(barrier0),NULL,global_thread_number);
 pthread_mutex_init(&( question_time),NULL);
//----------------------------------------------------------------------------------   
//Creation of all threads 
for(int i = 0 ; i<global_thread_number;i++){//n commentator 
struct args * threadA = (struct args *)malloc(sizeof(struct args));
threadA->id = i ; 
threadA->q = &que;

pthread_create(&comment[i],NULL,commentator_void, (void *)threadA   );

}	
pthread_create(&moderator,NULL,moderator_void, (&que));
pthread_create(&main_thread,NULL,breaking_news,NULL);


 pthread_join(main_thread, NULL);
 pthread_join(moderator, NULL);
 for(int i = 0 ; i<global_thread_number;i++){//n commentator 

pthread_join(comment[i],NULL);

}
 
 
pthread_mutex_destroy(&mutex0);
pthread_mutex_destroy(&breaking);
pthread_mutex_destroy(&mutex1);
pthread_mutex_destroy(&mutex2);
pthread_mutex_destroy(&mutex3);
pthread_mutex_destroy(&mutex4);
pthread_mutex_destroy(&mutex5);
pthread_mutex_destroy(&mutex6);
pthread_mutex_destroy(&mutexforwait);
pthread_mutex_destroy(&question_time);
pthread_mutex_destroy(&mutex3);
pthread_mutex_destroy(&mutex4);
 pthread_cond_destroy (&( condition0));
  pthread_cond_destroy(&( bcond));
  pthread_cond_destroy (&( bcond2));
pthread_cond_destroy (&( condition1));
 pthread_cond_destroy(&( question_asked));
 pthread_cond_destroy (&( all_enqueued));
 pthread_cond_destroy(&(wake_up));
 pthread_barrier_destroy(&( barrier));
  pthread_barrier_destroy(&(barrier0));
pthread_exit (NULL);
}





















void * moderator_void ( void* global_queue  ){
Queue * q= (Queue * )global_queue;  //typecast commentator queue
//for loop is for the number of question number
for(int i =0 ; i<global_question; i++){
//re-initilize the flag so that we know all threads came here
flg=0;
//GET TIME AND PRINT
//--------------------------------------------------------------
struct timeval  tv;
gettimeofday(&tv, NULL);
int mic_local = (tv.tv_usec/1000) ;
time_t now2 = time(NULL);
struct tm *tm_struct2 = localtime(&now2);
int hour = tm_struct2->tm_hour;
int min = tm_struct2->tm_min;
int sec = tm_struct2->tm_sec;
printf("\n[%d:%d:%d.%3d] Moderator asks question %d ",hour,min,sec,mic_local,i+1);
//-----------------------------------------------------------------
//to make sure that all threads are awake we broadcast them until all of them are awake
while(flg != global_thread_number){
pthread_cond_broadcast(&question_asked);
//printf(	"prb here");
}
//this sends the signal and states all threads are enqueued
pthread_cond_wait(&all_enqueued, &mutex3);
pthread_mutex_unlock(&mutex3);
int m = 0;
//this while loop is repeated until all queue is empty
while( counter !=0  ){
//this while loop is for to make sure the commentator thread is waken up
while(control_bit==0){
pthread_cond_signal(&wake_up);
}

//decrement the counter 
counter--;

//here moderator waits until the commentator finsihes  his speech
 pthread_cond_wait(&condition0, &question_time);
//this boo 3 is for re initilizing the boolean that used in commentator
boo3++;
 pthread_mutex_unlock(&question_time);
} 


//Moderator waits here until last commentator speak and after here it turns back to the starting of the for loop
pthread_cond_wait(&condition1, &mutex0);

//re initilize the boolean at the commentator
boo++;
//printf("son soz");
pthread_mutex_unlock(&mutex0 );

}
pthread_exit (NULL);
}





void * breaking_news (){

//this counter let us to track to end the breaking news program and also used as an index ofglobal queue
while(counter_b != -1){
//wait until new thread comes to speak
pthread_cond_wait(&bcond2, &mutex6);
//this if statement is true if there is a breaking news with the probobality that user typed
if(Breaking_News[counter_b] < (int)global_break  ){
//this global integer states there is a global breaking news
b_signalled=1;

//sends signal until the time commentattor conditioanlly waits
while(b_signalled==1){
pthread_cond_signal(&bcond);
}

//I used this while loop as a waiting statement before going back to start of the while loop it waits until the time breaking news is finished
while(finish_break==0);
finish_break=0;
}
pthread_mutex_unlock(&mutex6);
//if statement checks if it was the last question's last commentator
if(counter_b != -1){
counter_b++;
}
}
}
void * commentator_void (void *input  ){
// for loop parelel to the moderator
for(int i = 0 ; i <global_question; i++){

int id= ((struct args*)input)->id;
Queue * q= ((struct args*)input)->q;  //typecast commentator queue

// waits until all threads comes to start of the program
pthread_barrier_wait (&barrier);

//re initilize the boolean
boo4=0;
// this wait is broadcasted by the moderator
//this wait states that question is asked so every thread wakes up
pthread_cond_wait(&(question_asked), &(mutex1));
pthread_mutex_unlock(&mutex1);
//updates the flag which is used to count the number which is used by moderator
pthread_mutex_lock(&mutex2);
flg++;
pthread_mutex_unlock(&mutex2);

//make sure all threads are awaken
pthread_barrier_wait (&barrier);

//here we test all of the threads wether do they have question or not
//-------------------------------------------------------------------
pthread_mutex_lock(&mutex2);
int willask= possible_question(id,i);
if( willask == 1){//means commentator has a question
enqueue(q ,id);
counter++;
cou=counter;
}
pthread_mutex_unlock(&mutex2);
//----------------------------------------------------------------------
//wait until all threads are checked wether they have question or not
pthread_barrier_wait (&barrier);
pthread_cond_signal(&all_enqueued);


//this is the case when no one has a question no one in the queue and updadet by only one thread to avoid confisions to check it if only one thread updates it there is boo4
//----------------------------------------------------------------------
if(cou==0 & willask ==0 && boo4==0 ){
boo4=1;
control_bit=0;
      boo=0;
   turn=0;
   boo3=0;
	while(boo==0){
	pthread_cond_signal(&condition1);

	}

}
//----------------------------------------------------------------------


//threads who have question comes here
if( willask == 1){

//turns in this while loop until this threads turn comes
while(id != q->queue[turn]){

}

//this is to make sure that only one thread is waken up so other threads waits
pthread_mutex_lock(&mutexforwait);
pthread_cond_wait(&wake_up, &mutex5);

turn++;
control_bit=1;
//GET TIME AND PRINT
//----------------------------------------------------------------------
struct timeval  tv;
gettimeofday(&tv, NULL);
int mic_local = (tv.tv_usec/1000) ;
time_t now2 = time(NULL);
struct tm *tm_struct2 = localtime(&now2);
int hour = tm_struct2->tm_hour;
int min = tm_struct2->tm_min;
int sec = tm_struct2->tm_sec;
double speech =hourss[id][i]+1; 
printf("\n[%d:%d:%d.%3d]Commentator #%d's turn to speak for %f seconds",hour,min,sec,mic_local,id, speech);
//----------------------------------------------------------------------



//if it is the last answer of very last question it updates breaking news event 
if(counter==0){
    if(i+1==global_question) counter_b=-1;
}  
    finish_break=1;
pthread_cond_signal(&bcond2);
struct timeval tp;
    struct timespec timetoexpire;
    // When to expire is an absolute time, so get the current time and add
    // it to our delay time
    gettimeofday(&tp, NULL);
    long new_nsec = tp.tv_usec * 1000 + (speech - (long)speech) * 1e9;
    timetoexpire.tv_sec = tp.tv_sec + (long)speech + (new_nsec / (long)1e9);
    timetoexpire.tv_nsec = new_nsec % (long)1e9;

//for breaking news
pthread_cond_timedwait(&bcond,&breaking,&timetoexpire);

if( b_signalled==1){
struct timeval  tvs;
gettimeofday(&tvs, NULL);
int mic_localb = (tvs.tv_usec/1000) ;
time_t nowb23 = time(NULL);
struct tm *tm_structb2 = localtime(&nowb23);
int hourb = tm_structb2->tm_hour;
int minb = tm_structb2->tm_min;
int secb = tm_structb2->tm_sec;

printf("\n[%d:%d:%d.%3d] Breaking news",hourb,minb,secb,mic_localb);
printf("\n[%d:%d:%d.%3d]Commentator #%d is cut short due to a breaking news",hourb,minb,secb,mic_localb,id);
//sleeps when there is a breaking news
pthread_sleep(5);

//--------------------------------
struct timeval  stvs;
gettimeofday(&stvs, NULL);
int mic_localb2 = (stvs.tv_usec/1000) ;
time_t nowb2 = time(NULL);
struct tm *tm_structb = localtime(&nowb2);
int hourb2 = tm_structb->tm_hour;
int minb2 = tm_structb->tm_min;
int secb2 = tm_structb->tm_sec;

printf("\n[%d:%d:%d.%3d] Breaking news ends",hourb,minb,secb,mic_localb);

//----------------------------------------

}else{


//pthread_sleep(0.1);
struct timeval  tv2;
gettimeofday(&tv2, NULL);
int mic_local2 = (tv2.tv_usec/1000) ;
time_t now3 = time(NULL);
struct tm *tm_struct3 = localtime(&now3);
int hour2 = tm_struct3->tm_hour;
int min2 = tm_struct3->tm_min;
int sec2 = tm_struct3->tm_sec;
printf("\n[%d:%d:%d.%3d] Commentator #%d'finished speaking",hour2,min2,sec2,mic_local2,id);

}
pthread_mutex_unlock(&breaking);
b_signalled=0;

//this is the if for last commentator in any question 
if(counter==0){
    control_bit=0;
      boo=0;
   turn=0;
   boo3=0;
   while(boo3==0){
   
      pthread_cond_signal(&condition0);
}
	while(boo==0){
	pthread_cond_signal(&condition1);

	}
}
control_bit=0;
boo2=0;
pthread_cond_signal(&condition0);
pthread_mutex_unlock(&mutex5);
pthread_mutex_unlock(&mutexforwait);
}
}
pthread_exit (NULL);
}




void enqueue(Queue *q ,int id){//enqueues commentator thread with mutual exclusion	
//enques it by its own id and it is checked 
q->queue[counter]=id;
struct timeval  tv;
gettimeofday(&tv, NULL);
int mic_local = (tv.tv_usec/1000) ;
time_t now2 = time(NULL);
struct tm *tm_struct2 = localtime(&now2);
int hour = tm_struct2->tm_hour;
int min = tm_struct2->tm_min;
int sec = tm_struct2->tm_sec;
printf("\n[%d:%d:%d.%3d] Commentator #%d generates answer, position in queue %d ",hour,min,sec,mic_local,id,counter);
}
	


int  possible_question(int id , int index){
//comentators asks question with probability enterd by the user
pthread_mutex_lock(&mutex4);
    int random_number = state[id][index];
pthread_mutex_unlock(&mutex4);
//asks question if this condition holds probability of this condition to hold is typed by the user
if(random_number<(int)global_possibility)return 1;
return 0;
}	






int pthread_sleep(double seconds){
    pthread_mutex_t mutex;
    pthread_cond_t conditionvar;
    if(pthread_mutex_init(&mutex,NULL)){
        return -1;
    }
    if(pthread_cond_init(&conditionvar,NULL)){
        return -1;
    }

    struct timeval tp;
    struct timespec timetoexpire;
    // When to expire is an absolute time, so get the current time and add
    // it to our delay time
    gettimeofday(&tp, NULL);
    long new_nsec = tp.tv_usec * 1000 + (seconds - (long)seconds) * 1e9;
    timetoexpire.tv_sec = tp.tv_sec + (long)seconds + (new_nsec / (long)1e9);
    timetoexpire.tv_nsec = new_nsec % (long)1e9;

    pthread_mutex_lock(&mutex);
    int res = pthread_cond_timedwait(&conditionvar, &mutex, &timetoexpire);
    pthread_mutex_unlock(&mutex);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&conditionvar);

    //Upon successful completion, a value of zero shall be returned
    return res; 
}

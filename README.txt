
Group Partenrs : Doruk Özer / Efe Yılmaz

-----------------------------------------------
THE WAY HOW IT COMPILED AND RUNNED 
***You have to specify all the flags there is no default values if the flags didn't typed in********
gcc -pthread -o termi simulation.c 
./termi -n 4 -p 0.75 -q 5 -t 3 -b 0.05
------------------------------------------------
In our project every parts are working
No race condition in our code 
There is mutual exclusion in every part to prevent race condition
Our time output for part3 is organized in [hour,minute,second.milisecond] way which is quite same but more detailed than in the pdf
We described our code in a very detailed way in the comments of our code
We used a lot of mutexes global variables to control all threads and make sure main thread, commentator threads, and moderator thread is comminicating
We used some booleans in type int to check wether the signal is sent to thread which is waiting on the condition.
-------------------------------------------------------------
BRIEFLY IMPLEMENTATION (detailed version is in the source code)
Since random numbers are not thread unique we generated them in the program start
All generated random numbers stored in specific global array 
Our project is based on the communication of 3 threads(commentor,mainthread(breaking news threa),and moderator thread)
moderator thread and commentator has for loops which is controlled by number pf questions
Barriers and conditional waits are used both at the start and end of these for loops to make sure that moderator and commentator works in parallel manner
Breaking news thread has a while loop which is controlled by a global integer and it is updated at the very last answer of the very last commentator.
In commentator barriers used after checking every thread has a question or not. Barriers are also preferred at the beggining of the for loops
Moderator waits on a condition before new question is generated. It is signalled by the commentator who is the last in the queue
Queue stores the id's and it is checked by the commentator void if it his that commentators turn or not
We think most important part was to be sure that signals are sent properly to waiting threads and we used loops for that
Commentator uses conditionally timed wait in case of an interrupt by the main thread(breaking news thread) commentator's speech gets interrupted
We implemented the procedures in the pdf. Their roles are same with the pdf.
----------------------------------------------------------------------------------------------------------------------------------



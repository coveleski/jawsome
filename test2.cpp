#include "lib-ult.cpp" //the uthread library code is in lib-ult.c
#include <mutex>
#include <unistd.h>
#include <stdlib.h>

int n_threads=0;
int myid=0;

pthread_mutex_t mutex;

void do_something(){
	sleep(3);
	fprintf(stderr, "%s\n", "made it");
	pthread_mutex_lock(&mutex);
	int id;
	id=myid;
	myid++;
	pthread_mutex_unlock(&mutex);
	fprintf(stderr, "This is ult %d\n", id);

// 	if(n_threads<5){
// 		uthread_create(do_something,1);
// 		n_threads++;
// 		uthread_create(do_something,1);
// 		n_threads++;
// }
	// printf("This is ult %d again\n", id);
	// uthread_yield(2);
	printf("This is ult %d one more time\n", id);
	uthread_exit();
}

main(){
	pthread_mutex_init(&mutex, NULL);
	// do_something();
	int i;
	system_init(1); //only 1 kernel thread
	uthread_create(do_something,5);
	uthread_create(do_something,2);
	uthread_create(do_something,3);
	uthread_create(do_something,4);
	uthread_create(do_something,1);
	fprintf(stderr, "%s\n", "tasks queued");
	sleep(20);
	
}
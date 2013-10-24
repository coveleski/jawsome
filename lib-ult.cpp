//#define _GNU_SOURCE

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <queue>
#include <vector>
#include "dispatcher.h"

int loop(void *);



Dispatcher *d;

int loop(void *p){
	int *v = (int *)p;
	printf("%d\n", *v);
	fprintf(stderr, "asdf\n");

	return 1;
}


void system_init(int max_number_of_klt){

	if (max_number_of_klt < 1){
		return;
	}

	d = new Dispatcher(max_number_of_klt);

	//turn into an array of void pointers, i guess
	// char *child_stack_one, *child_stack_two;
	// child_stack_one = (char *) malloc(1001);
	// child_stack_two = (char *) malloc(1001);
	// child_stack_one+=1000;
	// child_stack_two+=1000;

	// clone(loop, child_stack_one, CLONE_VM, NULL);
	// clone(loop, child_stack_two, CLONE_VM, NULL);


	return;
}

int uthread_create(void func(), int pri_num){


	//make a context for the new function
	//makecontext();



}

int uthread_yield(int pri_num){
	//get new highest priority thread's context

	//update priority
	//move thread from running to ready

}

void uthread_exit(){
	//restore context to thread manager
	
}




int main(){
	fprintf(stderr, "%s\n", "running");
	system_init(2);

}

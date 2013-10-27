#include "dispatcher.h"
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <map>
#include <mutex>
#include <queue>
#include <vector>


int schedule(void *);
std::map<pthread_t, ucontext_t*> kthread_map;

//Mutex to keep the Priority Queue locked
pthread_mutex_t dispatcher_mutex;
//Mutex to keep the Return Context map locked
pthread_mutex_t map_mutex;
//A class to keep track of the task queue and running threads
Dispatcher *d;

/**
Gets the next task from the priority queue and schedules it
*/
int schedule(void *p){
	//erase me
	fprintf(stderr, "%s\n", "i'm in schedule!");
	//get this threads id
	pthread_t tid = pthread_self();
	//initialize a ucontext_t for my current context
	ucontext_t me;
	//set the context for this thread from the map, safely
	pthread_mutex_lock(&map_mutex);
	kthread_map[tid] = &me;
	pthread_mutex_unlock(&map_mutex);
	getcontext(&me);

	//lock the dispatcher until we've gotten a task
	pthread_mutex_lock(&dispatcher_mutex);
	//make sure there's tasks
	if (!d->pq.empty()){
		//pop a task and set the context to it's context
		Task task = d->pq.top();
		d->pq.pop();

		pthread_mutex_unlock(&dispatcher_mutex);
		//erase me
		// ucontext_t context = task.context;
		// free(task.context);

		//set uc_link for the task, so it returns to schedule
		task.context->uc_link = &me;
		//erase me
		fprintf(stderr, "%s\n", "jumping");


		setcontext(task.context);
	} else {
		//no tasks left, the clone will die
		//decrease current number of kernel threads
		d->cur_klt--;
		//make sure the mutex gets unlocked
		pthread_mutex_unlock(&dispatcher_mutex);
		return;
	}
}

/**
* Initializes the system for a given number of kthreads
*/
void system_init(int max_number_of_klt){
	//complain if the number of threads isn't positive
	if (max_number_of_klt < 1){
		return;
	}
	//init mutexes
	pthread_mutex_init(&map_mutex, NULL);
	pthread_mutex_init(&dispatcher_mutex, NULL);

	//erase me
	// getcontext(&me);

	//make a new dispatcher to deal with the user threads
	d = new Dispatcher(max_number_of_klt);

	return;
}

/**
* Creates a new thread with the given function and priority
*/
int uthread_create(void func(), int pri_num){
	//make a new context pointer for the new thread
	ucontext_t *context = new ucontext_t;

	//erase me?
	getcontext(context);

	//allocate a new stack for the context
	context->uc_stack.ss_sp=malloc(16384);
	context->uc_stack.ss_size=16384;
	//make the context with the given function
	makecontext(context, func,0);
	//wrap the context and priority in a Task
	Task t = Task(pri_num, context);
	//lock the Dispatcher and push the new task, safely
	pthread_mutex_lock(&dispatcher_mutex);
	d->pq.push(t);
	
	//check current number of spawned kthreads
	//if allowed, spawn another
	if (d->cur_klt < d->max_klt){
		//erase me
		fprintf(stderr, "spawning kthread %d\n", d->cur_klt);

		//make a stack for the new kthread
		char *child_stack_one;
		child_stack_one = (char *) malloc(100001);
		child_stack_one+=100000;
		d->cur_klt++;
		pthread_mutex_unlock(&dispatcher_mutex);

		//new kthread, running schedule()
		if (clone(schedule, child_stack_one, CLONE_VM, NULL)){
			//if clone succeeds, return 0
			return 0;
		} else {
			//if clone fails, return -1
			return -1;
		}
	} else{
		//make sure mutex unlocks
		pthread_mutex_unlock(&dispatcher_mutex);
		return 0;
	}

}

/**
* The running thread yields it's execution, and updates its priority
*/
int uthread_yield(int pri_num){
	//get the thread id
	pthread_t tid = pthread_self();
	//lock and look up thread context, safely
	pthread_mutex_lock(&map_mutex);
	ucontext_t *old = kthread_map[tid];
	pthread_mutex_unlock(&map_mutex);
	//make a new context for the thread's current location
	ucontext_t *context = new ucontext_t;
	//wrap new context and priority in a Task
	Task t = Task(pri_num, context);
	//lock and push the new task, safely
	pthread_mutex_lock(&dispatcher_mutex);
	d->pq.push(t);
	pthread_mutex_unlock(&dispatcher_mutex);
	//swap context back to schedule()
	return swapcontext(t.context, old);

}

/**
* Causes the thread to exit
*/
void uthread_exit(){
	//get thread id
	pthread_t tid = pthread_self();
	//lock and look up thread's old context
	//to return to schedule()
	pthread_mutex_lock(&map_mutex);
	ucontext_t *old = kthread_map[tid];
	pthread_mutex_unlock(&map_mutex);
	//set context to old context
	setcontext(old);
}

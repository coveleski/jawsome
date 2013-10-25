#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <queue>
#include <vector>
#include "dispatcher.h"
#include <map>
#include <pthread.h>
#include <mutex>

int schedule(void *);
std::map<pthread_t, ucontext_t*> kthread_map;

pthread_mutex_t dispatcher_mutex;
pthread_mutex_t map_mutex;
Dispatcher *d;

int schedule(void *p){
	fprintf(stderr, "%s\n", "i'm in schedule!");
	pthread_t tid = pthread_self();
	ucontext_t me;

	pthread_mutex_lock(&map_mutex);
	kthread_map[tid] = &me;
	pthread_mutex_unlock(&map_mutex);
	getcontext(&me);


	pthread_mutex_lock(&dispatcher_mutex);
	if (!d->pq.empty()){
		Task task = d->pq.top();
		d->pq.pop();

		pthread_mutex_unlock(&dispatcher_mutex);
		// ucontext_t context = task.context;
		// free(task.context);
		task.context->uc_link = &me;
		fprintf(stderr, "%s\n", "jumping");
		setcontext(task.context);
	} else {
		pthread_mutex_unlock(&dispatcher_mutex);
	}
}


void system_init(int max_number_of_klt){

	if (max_number_of_klt < 1){
		return;
	}

	pthread_mutex_init(&map_mutex, NULL);
	pthread_mutex_init(&dispatcher_mutex, NULL);

	// getcontext(&me);
	d = new Dispatcher(max_number_of_klt);

	return;
}

int uthread_create(void func(), int pri_num){

	ucontext_t *context = new ucontext_t;
	getcontext(context);
	context->uc_stack.ss_sp=malloc(16384);
	context->uc_stack.ss_size=16384;

	makecontext(context, func,0);
	Task t = Task(pri_num, context);

	pthread_mutex_lock(&dispatcher_mutex);
	d->pq.push(t);
	

	if (d->cur_klt < d->max_klt){
		fprintf(stderr, "spawning kthread %d\n", d->cur_klt);

		char *child_stack_one;
		child_stack_one = (char *) malloc(100001);
		child_stack_one+=100000;
		d->cur_klt++;
		pthread_mutex_unlock(&dispatcher_mutex);


		clone(schedule, child_stack_one, CLONE_VM, NULL);
	} else{
		pthread_mutex_unlock(&dispatcher_mutex);
	}

}

int uthread_yield(int pri_num){
	pthread_t tid = pthread_self();

	pthread_mutex_lock(&map_mutex);
	ucontext_t *old = kthread_map[tid];
	pthread_mutex_unlock(&map_mutex);

	ucontext_t *context = new ucontext_t;

	Task t = Task(pri_num, context);

	pthread_mutex_lock(&dispatcher_mutex);
	d->pq.push(t);
	pthread_mutex_unlock(&dispatcher_mutex);

	swapcontext(t.context, old);

}

void uthread_exit(){
	pthread_t tid = pthread_self();

	pthread_mutex_lock(&map_mutex);
	ucontext_t *old = kthread_map[tid];
	pthread_mutex_unlock(&map_mutex);

	setcontext(old);
}

// int main(){
// 	system_init(1);
// 	uthread_create(4)
// }

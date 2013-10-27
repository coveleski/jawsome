#include <ucontext.h>
#include <queue>
#include <vector>
#include <time.h>

/**
A container class to hold the thread context, priority, and ready_time, while in the queue
*/
 class Task{
	public:
		//Constructor
		Task(int pri, ucontext_t *con);
		//The task's priority
		int priority;
		//The time the task became ready
		time_t ready_time;
		//pointer to the thread's context
		ucontext_t *context;
	
};

//A comparator, to determine which Task has higher priority
class TaskCompare{
	public:
		//A constructor
		TaskCompare(const bool& revparam=false);
		//Overloaded compare operator
		bool operator() (const Task& lhs, const Task& rhs);
	private:
		//signals which order to sort in
		bool reverse;
};

//Typedef a Priority Queue of tasks, compared using  TaskCompare
typedef std::priority_queue<Task,std::vector<Task>,TaskCompare> TaskPQ;

//A class containing necessary items to schedule user threads
class Dispatcher {

	public:	
		//Dispatcher constructor
		Dispatcher(int max);
		//A priority Queue of tasks
		TaskPQ pq;
		//the current number of spawned kernel threads
		int cur_klt;
		//the number of max kernel threads
		int max_klt;
};

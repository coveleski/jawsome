#include <ucontext.h>
#include <queue>
#include <vector>
#include <time.h>


 class Task{
	public:
		int priority;
		time_t time;
		ucontext_t context;
	
};

class TaskCompare{
	public:
		TaskCompare(const bool& revparam=false);
		bool operator() (const Task& lhs, const Task& rhs);
	private:
		bool reverse;
};
typedef std::priority_queue<Task,std::vector<Task>,TaskCompare> TaskPQ;

class Dispatcher {

	public:
		Dispatcher(int max);
		TaskPQ pq;
		int cur_klt;
		int max_klt;
};

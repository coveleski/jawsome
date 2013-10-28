/**
* Authored by David Vriezen
* 27 October, 2013
* Com S 352 Project 1
*/

#include "dispatcher.h"
	/**
	*Task constructor
	* It's not very exciting
	*/
	Task::Task(int pri, ucontext_t *con){
		//just assigning values
		this->priority = pri;
		this->context = con;
		//task's creation time set to current time
		this->ready_time = time(NULL);
	}

	/**
	*Dispatcher constructor
	*initialize max_klt to max
	*initialize cur_klt to 0
	*/
	Dispatcher::Dispatcher(int max):max_klt(max),cur_klt(0){};

	/**
	* TaskCompare Constructor
	* Set reverse to revparam
	*/
	TaskCompare::TaskCompare(const bool& revparam):reverse(revparam){};

	bool TaskCompare::operator() (const Task& lhs, const Task& rhs){
		bool result = false;
			//if priorities are the same, tiebreak with ready_times
			if (lhs.priority == rhs.priority){
				result = lhs.ready_time < rhs.ready_time;
			} 
			//otherwise compare priorities
			else{

				result =  lhs.priority < rhs.priority;
			}
			//return appropriate values
			if (reverse){
				return result;
			}
			return !result;	
	}
#include "dispatcher.h"

	
	Dispatcher::Dispatcher(int max):max_klt(max),cur_klt(0){};

		TaskCompare::TaskCompare(const bool& revparam):reverse(revparam){};

	bool TaskCompare::operator() (const Task& lhs, const Task& rhs){
		bool result = false;

			if (lhs.priority == rhs.priority){
				result = lhs.time < rhs.time;
			} else{
				result =  lhs.priority < rhs.priority;
			}
			if (reverse){
				return !result;
			}
			return result;	
	}
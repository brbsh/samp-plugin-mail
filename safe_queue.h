#pragma once



#include "mail.h"





namespace boost
{
	template<typename T>
	class safe_queue
	{

	private:

		std::queue<T> queue;
		boost::mutex mutex;
		boost::condition_variable condition;

	public:

		void push(const T& data)
		{
			boost::lock_guard<boost::mutex> lock(mutex);
			queue.push(data);
			condition.notify_one();
		}

		T pop()
		{
			boost::mutex::scoped_lock lock(mutex);

			while(queue.empty())
				condition.wait(lock);

			T ret = queue.front();
			queue.pop();

			return ret;
		}

		bool empty()
		{
			return queue.empty();
		}

		int size()
		{
			return queue.size();
		}
	};
};
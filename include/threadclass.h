#pragma once

#include <vector>
#include <thread>
#include <algorithm>
#include <atomic>
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <iostream>

namespace Threading
{
	template<typename T>
	class threadsafe_queue
	{
	private:
		mutable std::mutex mut;
		std::queue<T> data_queue;
		std::condition_variable data_cond;
		std::atomic<bool> _done;

	public:
		threadsafe_queue():_done(false)
		{}
		threadsafe_queue(threadsafe_queue const& other)
		{
			std::lock_guard<std::mutex> lk(other.mut);
			data_queue = other.data_queue;
		}

		void push(T new_value)
		{
			std::lock_guard<std::mutex> lk(mut);
			data_queue.push(new_value);
			data_cond.notify_one();
		}

		void clear()
		{
			data_queue = {};
		}

		void wait_and_pop(T& value)
		{
			std::unique_lock<std::mutex> lk(mut);

			while (!_done && data_queue.empty())
			{
				data_cond.wait(lk, [this]{return _done || !data_queue.empty();});
			}

			if (_done)
				return;

			value = data_queue.front();
			data_queue.pop();
		}

		void stop()
		{
			_done = true;
			data_cond.notify_all();
		}

		std::shared_ptr<T> wait_and_pop()
		{
			std::unique_lock<std::mutex> lk(mut);

			while (!_done && data_queue.empty())
			{
				data_cond.wait(lk, [this]{return _done || !data_queue.empty();});
			}

			if (_done)
				return;

			std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
			data_queue.pop();

			return res;
		}
		
		bool try_pop(T & value)
		{
			std::lock_guard<std::mutex> lk(mut);

			if (data_queue.empty())
				return false;

			value = data_queue.front();
			data_queue.pop();

			return true;
		}

		std::shared_ptr<T> try_pop()
		{
			std::lock_guard<std::mutex> lk(mut);

			if (data_queue.empty())
				return std::shared_ptr<T>();

			std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
			data_queue.pop();

			return res;
		}

		bool empty() const
		{
			std::lock_guard<std::mutex> lk(mut);
			return data_queue.empty();
		}
	};

	class join_threads
	{
		std::vector<std::thread>& _threads;
	public:
		explicit join_threads(std::vector<std::thread>& threads_) :
			_threads(threads_)
		{}
		~join_threads()
		{
			for (unsigned long i = 0; i < _threads.size(); ++i)
			{
				if (_threads[i].joinable())
					_threads[i].join();
			}
		}
	};
}
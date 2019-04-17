#pragma once

#include <vector>
#include <thread>
#include <algorithm>
#include <atomic>
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

namespace threading
{
	template<typename T> class concurrent_stack {
		struct Node 
		{ 
			T t; 
			std::shared_ptr<Node> _next; 
		};

		std::shared_ptr<Node> _head{ nullptr };
		// in C++11: remove “atomic_” and remember to use _the special
		// functions every time you touch the variable
		concurrent_stack(concurrent_stack&) = delete;
		void operator=(concurrent_stack&) = delete;

	public:
		concurrent_stack() = default;
		~concurrent_stack() = default;
		class reference {
			std::shared_ptr<Node> _p;
		public:
			reference(std::shared_ptr<Node> p_) : _p{ p_ } { }
			T& operator* () { return _p->t; }
			T* operator->() { return &_p->t; }
		};

		auto find(T t) const {
			auto p = std::atomic_load(&_head);  // in C++11: atomic_load(&_head)
			while (p && p->t != t)
				p = p->_next;
			return reference(std::move(p));
		}
		auto front() const {
			return reference(std::atomic_load(&_head)); // in C++11: atomic_load(&_head)
		}
		void push_front(T t) {
			auto p = std::make_shared<Node>();
			p->t = t;
			p->_next = std::atomic_load(&_head);         // in C++11: atomic_load(&_head)
			while (!std::atomic_compare_exchange_weak(&_head, &p->_next, p)) {}
			// in C++11: atomic_compare_exchange_weak(&_head, &p->_next, p);
		}
		void pop_front() {
			auto p = std::atomic_load(&_head);
			while (p && !std::atomic_compare_exchange_weak(&_head, &p, p->_next)) {}
			// in C++11: atomic_compare_exchange_weak(&_head, &p, p->_next);
		}
	};

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
				data_cond.wait(lk, [this]{return !_done || !data_queue.empty();});
			}

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
				data_cond.wait(lk, [this]{return !_done || !data_queue.empty();});
			}

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

	unsigned long const Hardwarethreads =
		std::thread::hardware_concurrency();
	unsigned long Maxthreads = 2; // for scalability if there are millions of contacts/requests we can scale
	                               // by increasing the number of threads
	unsigned long const num_threads =
		std::min(Hardwarethreads != 0 ? Hardwarethreads : 2, Maxthreads);
}
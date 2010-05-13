
#ifndef HTTP_HANDLER_QUEUE
#define HTTP_HANDLER_QUEUE

#include <deque>

#include <boost/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>

namespace http
{

template<typename T>
class handler_queue_manager
{
public:
	handler_queue_manager() :
		handling_(false)
	{}
	
	void add_handler(T h) const
	{ 
		handler_queue_.push_back(h);
	}
	
	T get_current_handler() 
	{ 
		return handler_queue_.front(); 
	}
	
	bool can_start_handling_then_do()
	{
		boost::mutex::scoped_lock l(mutex_);
		
		if (!handler_queue_.empty() && !handling_)
			return handling_ = true;
		
		return false;
	}
	
	void handled()
	{
		boost::mutex::scoped_lock l(mutex_);
		
		handler_queue_.pop_front();
		handling_ = false;
	}
	
private:
	bool handling_;
	mutable std::deque<T> handler_queue_;
	
	mutable boost::mutex mutex_;
};

} // namespace http

#endif // HTTP_HANDLER_QUEUE

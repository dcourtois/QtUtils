#ifndef QT_UTILS_THREAD_H
#define QT_UTILS_THREAD_H

#include "./Setup.h"

#include <QRunnable>

#include <functional>


QT_UTILS_NAMESPACE_BEGIN

	//!
	//! Generic job class, using the global thread pool to run arbitrary jobs.
	//! Those threads are automatically deleted by the thread pool after they
	//! have finished executing, so to execute something on another thread,
	//! just do this:
	//!
	//! ```.cpp
	//! new Job([] (void) {
	//! 	// stuff here will happen on another thread
	//! });
	//! ```
	//!
	class Job
		: public QRunnable
	{

	public:

		Job(const std::function< void (void) > & job);

	protected:

		void run(void) final;

	private:

		//! The job function
		std::function< void (void) > m_Job;

	};

QT_UTILS_NAMESPACE_END


#endif

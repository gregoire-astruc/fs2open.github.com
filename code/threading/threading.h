
#ifndef THREADING_H
#define THREADING_H
#pragma once

#include "globalincs/pstypes.h"

#include <boost/threadpool.hpp>

namespace threading
{
	using namespace boost;

	namespace tp = threadpool;

	enum class WorkType
	{
		COLLISION
	};

	class WorkGroup
	{
	private:
		shared_ptr<tp::pool> mWorkPool;

	public:
		WorkGroup() {}

		WorkGroup(size_t threads);

		~WorkGroup() {}

		void submitTask(const tp::pool::task_type& task);

		void waitForTasks();
	};

	class ThreadingManager
	{
	private:
		SCP_map<WorkType, WorkGroup> mWorkGroups;

	public:
		ThreadingManager();

		~ThreadingManager() {}

		inline WorkGroup& getGroup(WorkType type)
		{
			Assertion(mWorkGroups.find(type) != mWorkGroups.end(), "Threading work type not found, get a coder!");

			return mWorkGroups[type];
		}
	};

	void init();

	ThreadingManager* manager();

	void shutdown();
}

#endif // THREADING_H

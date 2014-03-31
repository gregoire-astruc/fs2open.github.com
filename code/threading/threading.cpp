
#include <numeric>

#include "threading/threading.h"

#include <boost/scoped_ptr.hpp>

namespace
{
	using namespace boost;
	using namespace threading;

	struct GroupSettings
	{
		WorkType type;

		size_t weight;
	};

	std::vector<GroupSettings> workGroupSettings = {
		{ WorkType::COLLISION, 1 }
	};

	scoped_ptr<ThreadingManager> threadingManager;
}

namespace threading
{
	WorkGroup::WorkGroup(size_t threads) : mWorkPool(shared_ptr<tp::pool>(new tp::pool(threads)))
	{
	}

	void WorkGroup::submitTask(const tp::pool::task_type& task)
	{
		Assertion(mWorkPool, "Work pool no initialized, object was wrongly constructed!");

		mWorkPool->schedule(task);
	}

	void WorkGroup::waitForTasks()
	{
		Assertion(mWorkPool, "Work pool no initialized, object was wrongly constructed!");

		mWorkPool->wait();
	}

	ThreadingManager::ThreadingManager()
	{
		size_t weightSum = std::accumulate(std::begin(workGroupSettings), std::end(workGroupSettings), 0,
			[](size_t current, const GroupSettings& b) -> size_t { return current + b.weight; });

		auto cores = thread::hardware_concurrency();
		for (auto& groupSetting : workGroupSettings)
		{
			double relative = static_cast<double>(groupSetting.weight) / weightSum;
			auto numThreads = static_cast<size_t>(cores * relative);

			mWorkGroups.insert(std::make_pair(groupSetting.type, WorkGroup(numThreads)));
		}
	}

	void init()
	{
		threadingManager.reset(new ThreadingManager());
	}

	ThreadingManager* manager()
	{
		return threadingManager.get();
	}

	void shutdown()
	{
		threadingManager.reset();
	}
}

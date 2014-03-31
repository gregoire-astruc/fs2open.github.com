
SET(SOURCES
	dummy.cpp
	boost/threadpool.hpp
	boost/threadpool/future.hpp
	boost/threadpool/pool.hpp
	boost/threadpool/pool_adaptors.hpp
	boost/threadpool/scheduling_policies.hpp
	boost/threadpool/shutdown_policies.hpp
	boost/threadpool/size_policies.hpp
	boost/threadpool/task_adaptors.hpp
	boost/threadpool/detail/future.hpp
	boost/threadpool/detail/locking_ptr.hpp
	boost/threadpool/detail/pool_core.hpp
	boost/threadpool/detail/scope_guard.hpp
	boost/threadpool/detail/worker_thread.hpp
)

source_group(threadpool REGULAR_EXPRESSION boost/threadpool/.*)
source_group(threadpool\\detail REGULAR_EXPRESSION boost/threadpool/detail/.*)

add_library(threadpool STATIC ${SOURCES})

set_target_properties(threadpool
	PROPERTIES
		FOLDER "3rdparty"
)

IF(MSVC)
	IF(MSVC_USE_RUNTIME_DLL)
		SET(Boost_USE_STATIC_RUNTIME FALSE)
	ELSE(MSVC_USE_RUNTIME_DLL)
		SET(Boost_USE_STATIC_RUNTIME TRUE)
	ENDIF(MSVC_USE_RUNTIME_DLL)
ENDIF(MSVC)

SET(Boost_USE_STATIC_LIBS ON)

find_package(Boost COMPONENTS thread system date_time chrono REQUIRED)

TARGET_INCLUDE_DIRECTORIES(threadpool PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
TARGET_INCLUDE_DIRECTORIES(threadpool PUBLIC ${Boost_INCLUDE_DIRS})

TARGET_LINK_LIBRARIES(threadpool ${Boost_LIBRARIES})

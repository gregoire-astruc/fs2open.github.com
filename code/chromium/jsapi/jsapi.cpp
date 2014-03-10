
#include <functional>
#include <set>

#include "chromium/jsapi/jsapi.h"

#include "globalincs/pstypes.h"
#include "cfile/cfile.h"

#include "include/cef_task.h"
#include "include/cef_runnable.h"

#include <boost/algorithm/string.hpp>

#include <boost/bind.hpp>
#include <boost/detail/container_fwd.hpp>

#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>

namespace chromium
{
	namespace jsapi
	{
		SCP_map<CefString, FunctionType> functionList;
		boost::mutex functionListMutex;

		void init()
		{
			// Possibly initialize things here...
		}

		void addFunction(const CefString& name, const FunctionType& apiFunction)
		{
			boost::lock_guard<boost::mutex> guard(functionListMutex);

			functionList.insert(std::make_pair(name, apiFunction));
		}

		void removeFunction(const CefString& name)
		{
			boost::lock_guard<boost::mutex> guard(functionListMutex);

			functionList.erase(name);
		}

		void getFunctionNames(SCP_vector<CefString>& outVec)
		{
			boost::lock_guard<boost::mutex> guard(functionListMutex);

			outVec.clear();

			for (auto& pair : functionList)
			{
				outVec.push_back(pair.first);
			}
		}

		void functionExecutor(FunctionType func, CefRefPtr<CefBrowser> browser,
			int id, CefString name, CefRefPtr<CefListValue> args)
		{			
			CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create(API_MESSAGE_NAME);

			CefRefPtr<CefListValue> argList = message->GetArgumentList();

			argList->SetInt(0, id); // Query ID

			bool success = func(name, args, 2, argList);

			argList->SetBool(1, success); // Successful?

			browser->SendProcessMessage(PID_RENDERER, message);
		}

		void errorExecutor(CefRefPtr<CefBrowser> browser, int id)
		{
			CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create(API_MESSAGE_NAME);

			CefRefPtr<CefListValue> argList = message->GetArgumentList();

			argList->SetInt(0, id); // Query ID
			argList->SetBool(1, false); // Executed?
			argList->SetString(2, "API function is not known, the chomiumprocess executable is probably not compatible with the FSO executable.");

			browser->SendProcessMessage(PID_RENDERER, message);
		}

		bool processRendererMessage(CefRefPtr<CefBrowser> browser, CefRefPtr<CefProcessMessage> message)
		{
			CefRefPtr<CefListValue> list = message->GetArgumentList();

			int id = list->GetInt(0);
			CefString name = list->GetString(1);
			CefRefPtr<CefListValue> arguments = list->GetList(2)->Copy();

			auto iter = functionList.find(name);

			CefRefPtr<CefTask> task;
			bool retVal;
			if (iter == functionList.end())
			{
				task = NewCefRunnableFunction(errorExecutor, browser, id);
				retVal = false;
			}
			else
			{
				task = NewCefRunnableFunction(functionExecutor, iter->second, browser, id, name, arguments);
				retVal = true;
			}

			CefTaskRunner::GetForThread(TID_UI)->PostTask(task);

			return retVal;
		}
	}
}

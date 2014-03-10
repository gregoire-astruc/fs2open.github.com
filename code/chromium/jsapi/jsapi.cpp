
#include <functional>
#include <set>

#include "jsapi.h"

#ifndef BUILDING_CHROMIUMPROCESS
// Add includes for execution here

#include "globalincs/pstypes.h"
#include "cfile/cfile.h"

#include "include/cef_task.h"
#include "include/cef_runnable.h"

#endif

#include <boost/algorithm/string.hpp>

#include <boost/bind.hpp>
#include <boost/detail/container_fwd.hpp>

namespace chromium
{
	namespace jsapi
	{
#ifdef BUILDING_CHROMIUMPROCESS
		typedef std::set<CefString> ContainerType;
#else
		typedef SCP_map<CefString, FunctionType> ContainerType;
#endif

		ContainerType functionList;

#ifdef BUILDING_CHROMIUMPROCESS
#define ADD_FUNCTION(name, executionFunc) functionList.insert(name)
#else
#define ADD_FUNCTION(name, executionFunc) functionList.insert(std::make_pair(name, executionFunc))
#endif

		void cfile_init()
		{
			ADD_FUNCTION("cfile_listFiles", [](const CefString&, CefRefPtr<CefListValue> args, int index, CefRefPtr<CefListValue> outArgs)
			{
				CefRefPtr<CefDictionaryValue> argumentDict = args->GetDictionary(0);

				CefString dir = argumentDict->GetString("dir");
				cfile::SortMode sortMode = cfile::SORT_NONE;

				if (argumentDict->HasKey("sort"))
				{
					CefString sort = argumentDict->GetString("sort");

					if (boost::iequals(sort.c_str(), "time"))
					{
						sortMode = cfile::SORT_TIME;
					}
					else if (boost::iequals(sort.c_str(), "name"))
					{
						sortMode = cfile::SORT_NAME;
					}
				}

				SCP_vector<SCP_string> fileNames;
				cfile::listFiles(fileNames, dir.ToString().c_str(), "", sortMode);

				CefRefPtr<CefListValue> list = CefListValue::Create();

				size_t size = fileNames.size();
				list->SetSize(size);

				for (size_t i = 0; i < size; ++i)
				{
					list->SetString(i, fileNames[i].c_str());
				}

				outArgs->SetList(index, list);

				return true;
			});
		}

		void init()
		{
			cfile_init();
		}

#ifdef BUILDING_CHROMIUMPROCESS
		bool hasFunction(const CefString& name)
		{
			return functionList.find(name) != functionList.end();
		}

		void addAPIFunction(const CefString& name)
		{
			functionList.insert(name);
		}
#else
		void addFunction(const CefString& name, const FunctionType& apiFunction)
		{
			functionList.insert(std::make_pair(name, apiFunction));
		}

		void removeFunction(const CefString& name)
		{
			functionList.erase(name);
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

			ContainerType::iterator iter = functionList.find(name);

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
#endif
	}
}


#include <functional>

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

namespace chromium
{
	namespace jsapi
	{
#ifdef BUILDING_CHROMIUMPROCESS
		typedef std::map<CefString, FunctionType> ContainerType;
#else
		typedef SCP_map<CefString, FunctionType> ContainerType;
#endif

		ContainerType functionList;

#ifdef BUILDING_CHROMIUMPROCESS
#define ADD_FUNCTION(name, validationFunc, executionFunc) functionList.insert(std::make_pair(name, validationFunc))
#else
#define ADD_FUNCTION(name, validationFunc, executionFunc) functionList.insert(std::make_pair(name, executionFunc))
#endif

		void cfile_init()
		{
			ADD_FUNCTION("cfile_listFiles", [](const CefString& name, CefRefPtr<CefV8Value> argument, CefString& exception)
			{
				if (!argument->IsObject())
				{
					exception = "Argument must be an object!";
					return false;
				}

				CefRefPtr<CefV8Value> dirArg = argument->GetValue("dir");
				if (!dirArg.get() || !dirArg->IsString())
				{
					exception = "Need 'dir' arg and must be a string!";
					return false;
				}

				CefRefPtr<CefV8Value> fullPathsArg = argument->GetValue("fullPaths");
				if (fullPathsArg.get() && !fullPathsArg->IsUndefined())
				{
					if (!fullPathsArg->IsBool())
					{
						exception = "'fullPaths' must be a boolean!";
						return false;
					}
				}

				CefRefPtr<CefV8Value> sortArg = argument->GetValue("sort");
				if (dirArg.get() && !sortArg->IsUndefined())
				{
					// We have a sort argument
					if (!sortArg->IsString())
					{
						exception = "'sort' argument must be of type string!";
						return false;
					}

					CefString val = sortArg->GetStringValue();

					// Validate the sort argument here
					if (!boost::iequals(val.c_str(), "name")
						&& !boost::iequals(val.c_str(), "time"))
					{
						exception = "Unknown sort type specified!";
						return false;
					}
				}

				return true;
			}, [](const CefString&, CefRefPtr<CefListValue> args, int index, CefRefPtr<CefListValue> outArgs)
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
		bool validateQuery(const CefString& name, CefRefPtr<CefV8Value> argument, CefString& exception)
		{
			ContainerType::iterator iter = functionList.find(name);

			if (iter == functionList.end())
			{
				exception = std::string("No API function with name '") + name.ToString() + "' exists!";
				return false;
			}

			const FunctionType& func = iter->second;

			if (func)
			{
				return func(name, argument, exception);
			}
			else
			{
				// We know this function but don't have a validation function, let the browser process handle validation
				return true;
			}
		}

		void addUnvalidatedFunction(const CefString& name)
		{
			functionList.insert(std::make_pair(name, FunctionType(nullptr)));
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

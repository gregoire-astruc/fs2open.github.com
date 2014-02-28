
#include "Application.h"

#include "jsapi.h"

#include <boost/thread/lock_guard.hpp>

namespace
{
	const char* const fieldName_Name = "name";

	const char* const fieldName_Args = "args";

	const char* const fieldName_Callback = "callback";

	int nextId = 0;

	template<typename Index, typename List>
	bool setV8Value(const Index& index, List list, CefRefPtr<CefV8Value> value, CefString& exception)
	{
		if (value->IsArray())
		{
			CefRefPtr<CefListValue> newList = CefListValue::Create();
			newList->SetSize(value->GetArrayLength());

			for (int i = 0; i < value->GetArrayLength(); ++i)
			{
				if (!setV8Value(i, newList, value->GetValue(i), exception))
				{
					return false;
				}
			}

			list->SetList(index, newList);
		}
		else if (value->IsBool())
		{
			list->SetBool(index, value->GetBoolValue());
		}
		else if (value->IsDouble())
		{
			list->SetDouble(index, value->GetDoubleValue());
		}
		else if(value->IsInt())
		{
			list->SetInt(index, value->GetIntValue());
		}
		else if (value->IsNull())
		{
			list->SetNull(index);
		}
		else if (value->IsString())
		{
			list->SetString(index, value->GetStringValue());
		}
		else if (value->IsUInt())
		{
			list->SetInt(index, value->GetUIntValue());
		}
		else if (value->IsUndefined())
		{
			list->SetNull(index);
		}
		else if (value->IsObject())
		{
			std::vector<CefString> keys;

			if (!value->GetKeys(keys))
			{
				exception = "Failed to get object keys!";
				return false;
			}

			CefRefPtr<CefDictionaryValue> dictionary = CefDictionaryValue::Create();

			for (const CefString& key : keys)
			{
				if (!setV8Value(key, dictionary, value->GetValue(key), exception))
				{
					return false;
				}
			}

			list->SetDictionary(index, dictionary);
		}
		else
		{
			exception = "The specified argument cannot be passed to a query!";
			return false;
		}

		return true;
	}

	template<typename Index, typename List>
	CefRefPtr<CefV8Value> getV8Value(const Index& index, List list)
	{
		CefValueType type = list->GetType(index);

		switch (type)
		{
		case VTYPE_NULL:
			return CefV8Value::CreateNull();
		case VTYPE_BOOL:
			return CefV8Value::CreateBool(list->GetBool(index));
		case VTYPE_INT:
			return CefV8Value::CreateInt(list->GetInt(index));
		case VTYPE_DOUBLE:
			return CefV8Value::CreateDouble(list->GetDouble(index));
		case VTYPE_STRING:
			return CefV8Value::CreateString(list->GetString(index));
		case VTYPE_DICTIONARY:
		{
			CefRefPtr<CefDictionaryValue> dictionaryVal = list->GetDictionary(index);
			CefRefPtr<CefV8Value> v8Obj = CefV8Value::CreateObject(nullptr);

			CefDictionaryValue::KeyList keys;
			dictionaryVal->GetKeys(keys);

			for (auto& key : keys)
			{
				v8Obj->SetValue(key, getV8Value(key, dictionaryVal), V8_PROPERTY_ATTRIBUTE_NONE);
			}

			return v8Obj;
		}
		case VTYPE_LIST:
		{
			CefRefPtr<CefListValue> listVal = list->GetList(index);
			CefRefPtr<CefV8Value> v8List = CefV8Value::CreateArray(listVal->GetSize());

			for (int i = 0; i < (int)listVal->GetSize(); ++i)
			{
				v8List->SetValue(i, getV8Value(i, listVal));
			}

			return v8List;
		}
		default:
			return CefV8Value::CreateString("Invalid message value type, please report!");
		}
	}
}

class FunctionHandler : public CefV8Handler
{
private:
	CefRefPtr<CefProcessMessage> constructMessage(const CefString& name, CefRefPtr<CefV8Value> arg, int& id, CefString& exception)
	{
		id = nextId++;

		CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create(chromium::jsapi::API_MESSAGE_NAME);

		CefRefPtr<CefListValue> argumentList = message->GetArgumentList();
		argumentList->SetSize(3);

		argumentList->SetInt(0, id);
		argumentList->SetString(1, name);

		// We wrap the argument into a list so we have a uniform interface in the browser process
		CefRefPtr<CefListValue> argumentWrapper = CefListValue::Create();
		argumentWrapper->SetSize(1);

		if (!setV8Value(0, argumentWrapper, arg, exception))
		{
			return nullptr;
		}
		else
		{
			argumentList->SetList(2, argumentWrapper);
			return message;
		}
	}

	CefRefPtr<Application> application_;
public:
	FunctionHandler(CefRefPtr<Application> application) : application_(application)
	{}

	virtual bool Execute(const CefString& name, CefRefPtr<CefV8Value> object,
		const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception)
	{
		if (name == "queryFunc")
		{
			if (arguments.size() != 1 || !arguments.at(0)->IsObject())
			{
				exception = "A query needs exectly one object paramter!";
				return true;
			}

			CefRefPtr<CefV8Value> queryArg = arguments.at(0);

			CefRefPtr<CefV8Value> nameValue = queryArg->GetValue(fieldName_Name);
			if (!nameValue.get() || !nameValue->IsString())
			{
				exception = std::string("Query needs a field named '") + fieldName_Name + "' which is of type string!";
				return true;
			}

			CefRefPtr<CefV8Value> callbackValue = queryArg->GetValue(fieldName_Callback);
			if (!callbackValue.get() || !callbackValue->IsFunction())
			{
				exception = std::string("Query needs a field named '") + fieldName_Callback + "' which is of type function!";
				return true;
			}

			CefRefPtr<CefV8Value> argsValue = queryArg->GetValue(fieldName_Args);

			if (chromium::jsapi::validateQuery(nameValue->GetStringValue(), argsValue, exception))
			{
				int id;
				CefRefPtr<CefProcessMessage> message = constructMessage(nameValue->GetStringValue(), argsValue, id, exception);

				if (message.get())
				{
					CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();
					if (context->GetBrowser()->SendProcessMessage(PID_BROWSER, message))
					{
						// Save the callback and its ID
						application_->AddCallbackFunction(id, callbackValue, context);
					}
				}
				else
				{
					// Don't set a return value here so the exception gets thrown
					return true;
				}
			}

			retval = CefV8Value::CreateNull();

			return true;
		}

		return false;
	}

	IMPLEMENT_REFCOUNTING(FunctionHandler)
};

void Application::AddCallbackFunction(int id, CefRefPtr<CefV8Value> function, CefRefPtr<CefV8Context> context)
{
	// Make sure we are here only once
	boost::lock_guard<boost::mutex> guard(callbackMapLock);

	callbackMap.insert(std::make_pair(id, std::make_pair(function, context)));
}

bool Application::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
	if (message->GetName() == chromium::jsapi::API_MESSAGE_NAME)
	{
		auto argList = message->GetArgumentList();

		int id = argList->GetInt(0);
		bool successfull = argList->GetBool(1);

		// Lock the callback map so we get a consistens view
		boost::lock_guard<boost::mutex> guard(callbackMapLock);

		auto iter = callbackMap.find(id);

		if (iter == callbackMap.end())
		{
			// We don't know this ID, the context might already have been released
			return true;
		}

		auto& pair = iter->second;

		pair.second->Enter();

		CefRefPtr<CefV8Value> successVal = CefV8Value::CreateBool(successfull);
		CefRefPtr<CefV8Value> value = getV8Value(2, argList);

		CefV8ValueList list({ successVal, value });

		// We may not rethrow exceptions!
		pair.first->SetRethrowExceptions(false);
		pair.first->ExecuteFunction(nullptr, list);

		// If there was an exception there is nothing we can do about it so just clear it.
		pair.first->ClearException();

		pair.second->Exit();

		return true;
	}

	return false;
}

void Application::OnWebKitInitialized()
{
	// Register an extension which exposes the FSO API

	std::string extensionCode =
		"var fso;"
		"if (!fso)"
		"  fso = {};"
		"(function() {"
		"  fso.query = function(args) {"
		"    native function queryFunc(args);"
		"    return queryFunc(args);"
		"  };"
		"})();";

	// Create an instance of my CefV8Handler object.
	CefRefPtr<CefV8Handler> handler = new FunctionHandler(this);

	// Register the extension.
	CefRegisterExtension("fsoExtension", extensionCode, handler);

	chromium::jsapi::init();
}

void Application::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context)
{
	for (auto it = callbackMap.cbegin(); it != callbackMap.cend();)
	{
		if (it->second.second->IsSame(context))
		{
			// Erase all entries that use this context
			callbackMap.erase(it++);
		}
		else
		{
			++it;
		}
	}
}

void Application::OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info)
{
	// For future usage (probably lua <-> javascript bindings)
}


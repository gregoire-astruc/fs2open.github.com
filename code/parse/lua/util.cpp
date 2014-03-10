
#include "chromium/chromium.h"
#include "parse/lua/util.h"

#include <LuaCpp/LuaUtil.hpp>

#include <include/cef_v8.h>

namespace lua
{
	namespace util
	{
		using namespace luacpp;

		template<typename Index, typename List>
		void fromLuaValue(lua_State* L, const Index& index, List& list, const LuaValue& value)
		{
			switch (value.getValueType())
			{
			case ValueType::NIL:
				list->SetNull(index);
				break;
			case ValueType::BOOLEAN:
				list->SetBool(index, value.getValue<bool>());
				break;
			case ValueType::STRING:
				list->SetString(index, value.getValue<std::string>());
				break;
			case ValueType::NUMBER:
				list->SetDouble(index, value.getValue<double>());
				break;
			case ValueType::TABLE:
			{
				LuaTable table;
				table.setReference(value.getReference());

				SCP_vector<std::pair<std::string, LuaValue>> pairs;
				luacpp::util::tableListPairs(table, pairs);

				if (pairs.size() == table.getLength())
				{
					// this is an array
					CefRefPtr<CefListValue> listVal = CefListValue::Create();
					listVal->SetSize(pairs.size());

					int i = 0;
					for (auto& pair : pairs)
					{
						fromLuaValue(L, i, listVal, pair.second);

						++i;
					}

					list->SetList(index, listVal);
				}
				else
				{
					// this contains key-value pairs
					CefRefPtr<CefDictionaryValue> dictionaryVal = CefDictionaryValue::Create();

					for (auto& pair : pairs)
					{
						fromLuaValue(L, CefString(pair.first), dictionaryVal, pair.second);
					}

					list->SetDictionary(index, dictionaryVal);
				}
			}
			case ValueType::NONE:
			case ValueType::LIGHTUSERDATA:
			case ValueType::FUNCTION:
			case ValueType::USERDATA:
			case ValueType::THREAD:
			default:
				LuaError(L, "Cannot use value type '%s' for inter process communication!",
					luacpp::util::getValueName(value.getValueType()));
				break;
			}
		}

		template<typename Index, typename List>
		void toLuaValue(lua_State* L, const Index& index, const List& list, LuaValue& target)
		{
			CefValueType type = list->GetType(index);

			switch (type)
			{
			case VTYPE_NULL:
				target = LuaValue::createNil(L);
				break;
			case VTYPE_BOOL:
				target = LuaValue::createValue(L, list->GetBool(index));
				break;
			case VTYPE_INT:
				target = LuaValue::createValue(L, list->GetInt(index));
				break;
			case VTYPE_DOUBLE:
				target = LuaValue::createValue(L, list->GetDouble(index));
				break;
			case VTYPE_STRING:
				target = LuaValue::createValue(L, list->GetString(index).ToString());
				break;
			case VTYPE_DICTIONARY:
			{
				CefRefPtr<CefDictionaryValue> dictionaryVal = list->GetDictionary(index);
				LuaTable table = LuaTable::create(L);

				CefDictionaryValue::KeyList keys;
				dictionaryVal->GetKeys(keys);

				for (auto& key : keys)
				{
					LuaValue val;
					toLuaValue(L, key, dictionaryVal, val);

					table.addValue(key.ToString(), val);
				}

				target = table;
				break;
			}
			case VTYPE_LIST:
			{
				CefRefPtr<CefListValue> listVal = list->GetList(index);
				LuaTable table = LuaTable::create(L);

				for (int i = 0; i < (int)listVal->GetSize(); ++i)
				{
					LuaValue val;
					toLuaValue(L, i, listVal, val);

					table.addValue(i, val);
				}

				target = table;
				break;
			}
			default:
				target = LuaValue::createNil(L);
				break;
			}
		}

		void executeCallback(CefRefPtr<chromium::ClientImpl> client, const CefString& name, LuaTable& table)
		{
			if (!table.isValid())
			{
				client->executeCallback(name, CefListValue::Create());
				return;
			}

			SCP_vector<std::pair<std::string, LuaValue>> pairs;
			luacpp::util::tableListPairs(table, pairs);

			auto L = table.luaState;

			if (pairs.size() == table.getLength())
			{
				// this is an array
				CefRefPtr<CefListValue> listVal = CefListValue::Create();
				listVal->SetSize(pairs.size());

				int i = 0;
				for (auto& pair : pairs)
				{
					fromLuaValue(L, i, listVal, pair.second);

					++i;
				}

				client->executeCallback(name, listVal);
			}
			else
			{
				// this contains key-value pairs
				CefRefPtr<CefDictionaryValue> dictionaryVal = CefDictionaryValue::Create();

				for (auto& pair : pairs)
				{
					fromLuaValue(L, CefString(pair.first), dictionaryVal, pair.second);
				}

				client->executeCallback(name, dictionaryVal);
			}
		}

		struct LuaFunctionWrapper
		{
			LuaFunction mFunction;

			LuaFunctionWrapper(const LuaFunction& function) : mFunction(function) {}

			bool operator()(const CefString& name, CefRefPtr<CefListValue> arguments, int returnIndex, CefRefPtr<CefListValue> returnArgs)
			{
				LuaValue argument;
				toLuaValue(mFunction.luaState, 0, arguments, argument);

				LuaValueList returnVals = mFunction({ argument });

				if (!returnVals[0].is(ValueType::BOOLEAN))
				{
					LuaError(mFunction.luaState, "Callback function must return a boolean as first return value! Got type '%s'.",
						luacpp::util::getValueName(returnVals[0].getValueType()));

					returnArgs->SetString(returnIndex, "Lua callback error!");
					return false;
				}

				bool success = returnVals[0].getValue<bool>();

				fromLuaValue(mFunction.luaState, returnIndex, returnArgs, returnVals[1]);

				return success;
			}
		};

		void addApiFunction(const CefString& name, const LuaFunction& function)
		{
			chromium::jsapi::addFunction(name, LuaFunctionWrapper(function));
		}
	}
}

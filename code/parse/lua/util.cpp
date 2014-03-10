
#include "parse/lua/util.h"

#include <LuaCpp/LuaUtil.hpp>

namespace lua
{
	namespace util
	{
		using namespace luacpp;

		template<typename Index, typename List>
		void setLuaValue(lua_State* L, const Index& index, List list, LuaValue& value)
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
						setLuaValue(L, i, listVal, pair.second);

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
						setLuaValue(L, CefString(pair.first), dictionaryVal, pair.second);
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
					setLuaValue(L, i, listVal, pair.second);

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
					setLuaValue(L, CefString(pair.first), dictionaryVal, pair.second);
				}

				client->executeCallback(name, dictionaryVal);
			}
		}
	}
}

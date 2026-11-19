#include <nlohmann/json.hpp>
#include "ylang/vm.hpp"
using json = nlohmann::json;
#include <iostream>

extern "C" void YlangLoadModule(std::shared_ptr<Env> env, const std::vector<std::string> &symbols) {
    auto moduleNamespace = std::make_shared<ClassObject>("Json");
		moduleNamespace->mro.push_back(moduleNamespace.get());
		auto define = [&](string name, NativeFunc f) {
			moduleNamespace->staticFields[name] = Value::Native(f);
			if (symbols.size() == 1 && symbols[0] == "*") {
				env->set(name, Value::Native(f), true);
				return;
			}
			for (const auto &s : symbols)
				if (s == name) {
					env->set(name, Value::Native(f), true);
					break;
				}
		};
		static std::function<json(Value, int, int, std::vector<void *> &)>
			ValueToJson;
		static std::function<Value(json, int, int)> JsonToValue;
		static auto jsonClass = std::make_shared<ClassObject>("JsonObject");
		ValueToJson = [&](Value v, int l, int c,
							  std::vector<void *> &visited) -> json {
			if (v.type == ValueType::NONE)
				return nullptr;
			if (v.type == ValueType::BOOL)
				return v.asBool();
			if (v.type == ValueType::INT)
				return v.asInt();
			if (v.type == ValueType::FLOAT)
				return v.asFloat();
			if (v.type == ValueType::STRING)
				return v.asString();
			if (v.type == ValueType::LIST || v.type == ValueType::INSTANCE) {
				if (!v.ref)
					return nullptr;
				void *ptr = v.ref.get();
				for (void *p : visited)
					if (p == ptr)
						return "[Cyclic Reference]";
				visited.push_back(ptr);
				if (v.type == ValueType::LIST) {
					json j = json::array();
					auto list = static_cast<ListObject *>(ptr);
					for (auto &item : list->elements)
						j.push_back(ValueToJson(item, l, c, visited));
					visited.pop_back();
					return j;
				}
				if (v.type == ValueType::INSTANCE) {
					json j = json::object();
					auto inst = static_cast<InstanceObject *>(ptr);
					for (auto &pair : inst->fields) {
						if (pair.second.type != ValueType::INT &&
							 pair.second.type != ValueType::FLOAT &&
							 pair.second.type != ValueType::STRING &&
							 pair.second.type != ValueType::BOOL &&
							 pair.second.type != ValueType::LIST &&
							 pair.second.type != ValueType::INSTANCE &&
							 pair.second.type != ValueType::NONE) {
							continue;
						}
						j[pair.first] = ValueToJson(pair.second, l, c, visited);
					}
					visited.pop_back();
					return j;
				}
			}
			return nullptr;
		};
		JsonToValue = [&](json j, int l, int c) -> Value {
			if (j.is_null())
				return Value::None();
			if (j.is_boolean())
				return Value::Bool(j.get<bool>());
			if (j.is_number_integer())
				return Value::Int(j.get<long long>());
			if (j.is_number_float())
				return Value::Float(j.get<double>());
			if (j.is_string())
				return Value::String(j.get<std::string>());
			if (j.is_array()) {
				auto list = std::make_shared<ListObject>();
				for (auto &item : j) {
					list->elements.push_back(JsonToValue(item, l, c));
				}
				Value v;
				v.type = ValueType::LIST;
				v.ref = list;
				return v;
			}
			if (j.is_object()) {
				auto inst = std::make_shared<InstanceObject>(jsonClass.get());
				for (auto &[key, value] : j.items()) {
					inst->fields[key] = JsonToValue(value, l, c);
				}
				Value v;
				v.type = ValueType::INSTANCE;
				v.ref = inst;
				return v;
			}
			return Value::None();
		};
		define("ParseJson", [=](const vector<Value> &args, int l, int c) {
			if (args.size() != 1)
				throw ArgumentError("Json.Parse(string)", l, c);
			try {
				json j = json::parse(args[0].asString());
				return JsonToValue(j, l, c);
			} catch (json::parse_error &e) {
				throw ParseError(
					"Invalid JSON string at byte " + std::to_string(e.byte), l, c);
			}
		});
		define("StringifyJson", [=](const vector<Value> &args, int l, int c) {
			if (args.size() != 1 && args.size() != 2)
				throw ArgumentError("Json.stringify(object, indent = -1)", l, c);
			std::vector<void *> visited;
			json j = ValueToJson(args[0], l, c, visited);
			return Value::String(j.dump(args.size() == 2 ? args[1].asInt() : -1));
		});
		define("IsValidJson", [](const vector<Value> &args, int l, int c) {
			if (args.size() != 1)
				throw ArgumentError("Json.isValid(string)", l, c);
			bool isValid = json::accept(args[0].asString());
			return Value::Bool(isValid);
		});
		Value modVal;
		modVal.type = ValueType::CLASS;
		modVal.ref = moduleNamespace;
		env->set("Json", modVal, false, false);
	}

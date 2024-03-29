/*
* This file is part of the ungod - framework.
* Copyright (C) 2016 Felix Becker - fb132550@uni-greifswald.de
*
* This software is provided 'as-is', without any express or
* implied warranty. In no event will the authors be held
* liable for any damages arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute
* it freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented;
*    you must not claim that you wrote the original software.
*    If you use this software in a product, an acknowledgment
*    in the product documentation would be appreciated but
*    is not required.
*
* 2. Altered source versions must be plainly marked as such,
*    and must not be misrepresented as being the original software.
*
* 3. This notice may not be removed or altered from any
*    source distribution.
*/

#include "ungod/script/registration/RegisterSerialization.h"
#include "ungod/serialization/Serializable.h"
#include "ungod/serialization/MetaData.h"
#include "ungod/script/SerializationScriptContext.h"

namespace ungod
{
    namespace scriptRegistration
    {
        void registerSerialization(ScriptStateBase& state)
        {
			script::Usertype<MetaAttribute> metaAttrType = state.registerUsertype<MetaAttribute>("MetaAttribute");
			metaAttrType["next"] = sol::overload([](MetaAttribute& attr) { return attr.next(); },
				[](MetaAttribute& attr, const std::string& name) { return attr.next(name.c_str()); });
			metaAttrType["name"] = & MetaAttribute::name;
			metaAttrType["value"] = & MetaAttribute::value;
			metaAttrType["valueAsBool"] = [](MetaAttribute& attr) { return attr.convertValue<bool>(); };
			metaAttrType["valueAsInt"] = [](MetaAttribute& attr) { return attr.convertValue<int>(); };
			metaAttrType["valueAsFloat"] = [](MetaAttribute& attr) { return attr.convertValue<float>(); };
			metaAttrType["valueAsString"] = [](MetaAttribute& attr) { return attr.convertValue<std::string>(); };
			metaAttrType["valueAsUnsigned"] = [](MetaAttribute& attr) { return attr.convertValue<unsigned>(); };


			script::Usertype<MetaNode> metaNodeType = state.registerUsertype<MetaNode>("MetaNode");
			metaNodeType["firstNode"] = sol::overload([](MetaNode& node) { return node.firstNode(); },
					[](MetaNode& node, const std::string& name) { return node.firstNode(name.c_str()); });
			metaNodeType["firstAttribute"] = sol::overload([](MetaNode& node) { return node.firstAttribute(); },
				[](MetaNode& node, const std::string& name) { return node.firstAttribute(name.c_str()); });
			metaNodeType["next"] = sol::overload([](MetaNode& node) { return node.next(); },
				[](MetaNode& node, const std::string& name) { return node.next(name.c_str()); });
			metaNodeType["prev"] = sol::overload([](MetaNode& node) { return node.prev(); },
				[](MetaNode& node, const std::string& name) { return node.prev(name.c_str()); });
			metaNodeType["name"] = & MetaNode::name;
			metaNodeType["parent"] = &MetaNode::parent;
			metaNodeType["value"] = & MetaNode::value;
			metaNodeType["appendSubnode"] = & MetaNode::appendSubnode;


			script::Usertype<script::SerializationScriptContext> serializationContextType = state.registerUsertype<script::SerializationScriptContext>("SerializationScriptContext");
			serializationContextType["serializeString"] = [](script::SerializationScriptContext& context, const std::string& id, script::Environment env)
															{ context.serialize<std::string>(id, env); };
			serializationContextType["serializeInt"] = [](script::SerializationScriptContext& context, const std::string& id, script::Environment env)
															{ context.serialize<int>(id, env); };
			serializationContextType["serializeFloat"] = [](script::SerializationScriptContext& context, const std::string& id, script::Environment env)
															{ context.serialize<float>(id, env); };
			serializationContextType["serializeBool"] = [](script::SerializationScriptContext& context, const std::string& id, script::Environment env)
															{ context.serialize<bool>(id, env); };
			serializationContextType["serializeEntity"] = [](script::SerializationScriptContext& context, const std::string& id, script::Environment env)
															{ context.serialize<Entity>(id, env); };


			script::Usertype<script::DeserializationScriptContext> deserializationContextType = state.registerUsertype<script::DeserializationScriptContext>("DeserializationScriptContext");
			deserializationContextType["deserializeString"] = [](script::DeserializationScriptContext& context, const std::string& id, script::Environment env)
																{ return context.deserialize<std::string>(id, env); };
			deserializationContextType["deserializeBool"] = [](script::DeserializationScriptContext& context, const std::string& id, script::Environment env)
																{ return context.deserialize<bool>(id, env); };
			deserializationContextType["deserializeInt"] = [](script::DeserializationScriptContext& context, const std::string& id, script::Environment env)
																{ return context.deserialize<int>(id, env); };
			deserializationContextType["deserializeFloat"] = [](script::DeserializationScriptContext& context, const std::string& id, script::Environment env)
																{ return context.deserialize<float>(id, env); };
			deserializationContextType["deserializeEntity"] = [](script::DeserializationScriptContext& context, const std::string& id, script::Environment env)
																{ return context.deserialize<Entity>(id, env); };


			script::Usertype<MetaMap> metaMapType = state.registerUsertype<MetaMap>("MetaMap", sol::constructors<MetaMap(), MetaMap(const std::string id)>());
			metaMapType["load"] = [](MetaMap& metamap, const std::string& id) { metamap.load(id); };
			metaMapType["isLoaded"] = &MetaMap::isLoaded;
			metaMapType["getNodeWithKey"] = &MetaMap::getNodeWithKey;

			script::Usertype<MetaList> metaListType = state.registerUsertype<MetaList>("MetaList", sol::constructors<MetaList(), MetaList(const std::string id)>());
			metaListType["load"] = [](MetaList& metalist, const std::string& id) { metalist.load(id); };
			metaListType["isLoaded"] = & MetaList::isLoaded;
			metaListType["getNodeAt"] = & MetaList::getNodeAt;
			metaListType["getNodeCount"] = & MetaList::getNodeCount;

			script::Usertype<MetaTable> metaTableType = state.registerUsertype<MetaTable>("MetaTable", sol::constructors<MetaTable(), MetaTable(const std::string id)>());
			metaTableType["load"] = [](MetaTable& metatable, const std::string& id) { metatable.load(id); };
			metaTableType["isLoaded"] = & MetaTable::isLoaded;
			metaTableType["getNodeAt"] = & MetaTable::getNodeAt;
			metaTableType["getInnerSize"] = & MetaTable::getInnerSize;
			metaTableType["getOuterSize"] = & MetaTable::getOuterSize;


        }
    }
}

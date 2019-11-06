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

#include "ungod/serialization/SerialGameState.h"
#include "ungod/application/ScriptedGameState.h"
#include "ungod/application/Application.h"

namespace ungod
{
    void SerialBehavior<ScriptedGameState>::serialize(const ScriptedGameState& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeObject("c", data.mCamera, serializer);
        context.serializeObject("wg", data.mWorldGraph, serializer);
        context.serializeProperty("db", data.mRenderDebug, serializer);
    }

    void DeserialBehavior<ScriptedGameState>::deserialize(ScriptedGameState& data, MetaNode deserializer, DeserializationContext& context)
    {
        MetaAttribute attr = context.first(context.deserializeObject(data.mCamera, static_cast<sf::RenderTarget&>(data.getApp()->getWindow())), "c", deserializer);
        attr = context.next(context.deserializeObject(data.mWorldGraph, data), "wg", deserializer, attr);
        context.next(context.deserializeProperty(data.mRenderDebug, false), "db", deserializer, attr);
    }
}

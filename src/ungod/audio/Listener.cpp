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

#include "ungod/audio/Listener.h"
#include "ungod/visual/Camera.h"
#include "ungod/physics/Physics.h"
#include "ungod/base/World.h"

namespace ungod
{
    float GlobalListener::getScaling(const sf::Vector2f audioEmission, float cap) const
    {
        return 1.0f;
    }

    sf::Vector2f GlobalListener::getWorldPosition() const
    {
        return {0,0};
    }


    float CameraListener::getScaling(const sf::Vector2f audioEmission, float cap) const
    {
		return 1 - std::min(1.0f, distance(mWorld.getNode().mapToLocalPosition(mCamera.getCenter()), audioEmission) / cap);
    }

    sf::Vector2f CameraListener::getWorldPosition() const
    {
		return mWorld.getNode().mapToLocalPosition(mCamera.getCenter());
    }
}

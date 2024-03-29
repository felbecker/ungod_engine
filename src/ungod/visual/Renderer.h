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

#ifndef UNGOD_RENDERER_H
#define UNGOD_RENDERER_H

#include "ungod/base/Entity.h"
#include "ungod/base/Transform.h"
#include "ungod/physics/CollisionHandler.h"

namespace ungod
{
    class VisualsHandler;
    class VisualsComponent;
    class AnimationComponent;
    class ParticleSystemComponent;
    class MusicEmitterComponent;
    class LightEmitterComponent;
    class Application;

    /** \brief A "system" class that renders entities to a render target.
    * Also updates entities with animation components. */
    class Renderer
    {
    public:
        Renderer(Application& app);

        /** \brief Computes a new list of entities that intersect the render area. */
        void renewRenderlist(const quad::QuadTree<Entity>& entities, quad::PullResult<Entity>& pull, const sf::RenderTarget& target, sf::RenderStates states) const;

        /** \brief Draws the internal list of entities that must have a Transform and a Visual component and that are non-plane. */
        void render(const quad::PullResult<Entity>& pull, sf::RenderTarget& target, sf::RenderStates states, VisualsHandler& visualsHandler);

        /** \brief Draws the bounding boxes of all entities in the internal render-list. */
        void renderBounds(const quad::PullResult<Entity>& pull, sf::RenderTarget& target, sf::RenderStates states) const;

        /** \brief Draws the texture rects of all entities in the internal render-list, that have a Visuals-component and
        * either sprite-, vertex-array or multisprite-components. */
        void renderTextureRects(const quad::PullResult<Entity>& pull, sf::RenderTarget& target, sf::RenderStates states) const;

        /** \brief Draws the bounds of any attached particle-system. */
        void renderParticleSystemBounds(const quad::PullResult<Entity>& pull, sf::RenderTarget& target, sf::RenderStates states) const;

        /** \brief Draws the colliders of all entities in the internal render-list, that have a Rigidbody-component. */
        template<std::size_t CONTEXT = 0>
        void renderColliders(const quad::PullResult<Entity>& pull, 
            sf::RenderTarget& target, sf::RenderStates states,
            const sf::Color& outlineCol, const sf::Color& fillCol = sf::Color::Transparent) const;

        /** \brief Draws audio emitters. */
        void renderAudioEmitters(const quad::PullResult<Entity>& pull, sf::RenderTarget& target, sf::RenderStates states) const;

        /** \brief Draws light ranges. */
        void renderLightRanges(const quad::PullResult<Entity>& pull, sf::RenderTarget& target, sf::RenderStates states) const;

        /** \brief Updates the internal list of entities. Selects out the entities with Animation-components automatically.
        * Entities with no animation component will be skipped. */
        void update(const std::list<Entity>& entities, float delta, VisualsHandler& vh);

        /** \brief Renders an entity to the render target. The flip flag indicates whether the entity is rendering
        * is mirrored in y direction. This is used in water reflection-rendering. */
        void renderEntity(Entity e, TransformComponent& transf, VisualsComponent& vis, sf::RenderTarget& target, sf::RenderStates states, bool flip = false, float offsety = 0.0f);

        /** \brief Draws the bounds the given entity. */
        void renderBounds(const TransformComponent& transf, sf::RenderTarget& target, sf::RenderStates states) const;

        /** \brief Draws the texture rects the given entity. */
        void renderTextureRects(Entity e, const TransformComponent& transf, sf::RenderTarget& target, sf::RenderStates states) const;

        /** \brief Draws the texture rects the given entity. */
        void renderParticleSystemBounds(Entity e, const TransformComponent& transf, const ParticleSystemComponent& ps, sf::RenderTarget& target, sf::RenderStates states) const;

        /** \brief Draws the collider-bounds the given entity. */
        template<std::size_t CONTEXT = 0>
        void renderCollider(const TransformComponent& transf, const RigidbodyComponent<CONTEXT>& body, 
                                    sf::RenderTarget& target, sf::RenderStates states, 
                                    const sf::Color& outlineCol, const sf::Color& fillCol = sf::Color::Transparent) const;

        /** \brief Renders a audio emitter entity. */
        void renderAudioEmitter(Entity e, const TransformComponent& transf, MusicEmitterComponent& emitter, sf::RenderTarget& target, sf::RenderStates states) const;

        /** \brief Renders the origin and the range of a light. */
        void renderLightDebug(Entity e, const TransformComponent& transf, sf::RenderTarget& target, sf::RenderStates states) const;

        void resetDrawCalls() { mDrawCalls = 0; }

        int getDrawCalls() const { return mDrawCalls;  }

        static constexpr float INNER_RECT_PERCENTAGE = 0.1f;

    private:
        sf::RenderTexture mWaterTex;
        bool mShowWater;
        int mDrawCalls;

    private:
        void updateAnimation(Entity e, AnimationComponent& animation, float delta, VisualsHandler& vh);
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////IMPLEMENTATION////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<std::size_t CONTEXT>
    void Renderer::renderCollider(const TransformComponent& transf, const RigidbodyComponent<CONTEXT>& body, 
        sf::RenderTarget& target, sf::RenderStates states, const sf::Color& outlineCol, const sf::Color& fillCol) const
    {
        states.transform *= transf.getTransform();  //apply the transform of the entity
        switch (body.getCollider().getType())
        {
        case ColliderType::ROTATED_RECT:
        {
            RotatedRectConstAggregator c{ body.getCollider() };
            sf::RenderStates locstates = states;
            locstates.transform.rotate(c.getRotation(), c.getCenter());   //apply the rotation of the collider
            sf::RectangleShape rect(sf::Vector2f{ c.getDownRightX(), c.getDownRightY() } - sf::Vector2f{c.getUpLeftX(), c.getUpLeftY()});
            rect.setPosition({ c.getUpLeftX(), c.getUpLeftY() });
            rect.setFillColor(fillCol);
            rect.setOutlineThickness(3);
            rect.setOutlineColor(outlineCol);
            target.draw(rect, locstates);
            break;
        }
        case ColliderType::CONVEX_POLYGON:
        {
            PointSetConstAggregator ps{ body.getCollider() };
            sf::ConvexShape poly(ps.getNumberOfPoints());
            for (unsigned i = 0; i < ps.getNumberOfPoints(); i++)
                poly.setPoint(i, { ps.getPointX(i), ps.getPointY(i) });
            poly.setFillColor(fillCol);
            poly.setOutlineThickness(3);
            poly.setOutlineColor(outlineCol);
            target.draw(poly, states);
            break;
        }
        case ColliderType::EDGE_CHAIN:
        {
            PointSetConstAggregator ps{ body.getCollider() };
            sf::VertexArray lines(sf::LinesStrip, ps.getNumberOfPoints());
            for (unsigned i = 0; i < ps.getNumberOfPoints(); i++)
            {
                lines[i].position = { ps.getPointX(i), ps.getPointY(i) };
                lines[i].color = outlineCol;
            }
            target.draw(lines, states);
            break;
        }
        default:
            break;
        }
    }

    template<std::size_t CONTEXT>
    void Renderer::renderColliders(const quad::PullResult<Entity>& pull, 
        sf::RenderTarget& target, sf::RenderStates states, const sf::Color& outlineCol, const sf::Color& fillCol) const
    {
          dom::Utility<Entity>::iterate<TransformComponent, RigidbodyComponent<CONTEXT>>(pull.getList(),
			[this, &target, &states, outlineCol, fillCol] (Entity e, TransformComponent& transf, RigidbodyComponent<CONTEXT>& body)
			{
			    renderCollider(transf, body, target, states, outlineCol, fillCol);
			});
		  dom::Utility<Entity>::iterate<TransformComponent, MultiRigidbodyComponent<CONTEXT>>(pull.getList(),
			[this, &target, &states, outlineCol, fillCol](Entity e, TransformComponent& transf, MultiRigidbodyComponent<CONTEXT>& body)
			{
				  for (unsigned i = 0; i < body.getComponentCount(); i++)
					renderCollider(transf, body.getComponent(i), target, states, outlineCol, fillCol);
			});
    }
}

#endif // UNGOD_RENDERER_H

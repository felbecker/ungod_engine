#include "EntityTransformWindow.h"
#include "EditorFrame.h"
#include "wx/sizer.h"

namespace uedit
{

    wxBEGIN_EVENT_TABLE(EntityTransformWindow, wxWindow)
    wxEND_EVENT_TABLE()

    EntityTransformWindow::EntityTransformWindow(ungod::Entity e, ActionManager& actionManager, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz) :
        wxWindow(parent, id, pos, siz), mEntity(e), mActionManager(actionManager)
    {
        wxSizer* boxsizer = new wxBoxSizer(wxVERTICAL);

        {
            mPosX = new StatDisplay<float>("position x:", this, -1);
            mPosX->connectSetter( [this](float x)
            {
                mActionManager.transformActions().setEntityPosition(mEntity, { x, mEntity.get<ungod::TransformComponent>().getPosition().y });
            } );
            mPosX->connectGetter( [this]()
            {
                return mEntity.get<ungod::TransformComponent>().getPosition().x;
            } );
            boxsizer->Add(mPosX,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mPosY = new StatDisplay<float>("position y:", this, -1);
            mPosY->connectSetter( [this](float y)
            {
                mActionManager.transformActions().setEntityPosition(mEntity, { mEntity.get<ungod::TransformComponent>().getPosition().x, y });
            } );
            mPosY->connectGetter( [this]()
            {
                return mEntity.get<ungod::TransformComponent>().getPosition().y;
            } );
            boxsizer->Add(mPosY,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mScaleX = new StatDisplay<float>("scale x:", this, -1);
            mScaleX->connectSetter( [this](float scalex)
            {
                mActionManager.transformActions().setEntityScale(mEntity, { scalex, mEntity.get<ungod::TransformComponent>().getScale().y });
            } );
            mScaleX->connectGetter( [this]()
            {
                return mEntity.get<ungod::TransformComponent>().getScale().x;
            } );
            boxsizer->Add(mScaleX,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mScaleY = new StatDisplay<float>("scale y:", this, -1);
            mScaleY->connectSetter( [this](float scaley)
            {
                mActionManager.transformActions().setEntityScale(mEntity, { mEntity.get<ungod::TransformComponent>().getScale().x, scaley });
            } );
            mScaleY->connectGetter( [this]()
            {
                return mEntity.get<ungod::TransformComponent>().getScale().y;
            } );
            boxsizer->Add(mScaleY,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mBaseLineOffsetX = new StatDisplay<float>("base line offset left:", this, -1);
            mBaseLineOffsetX->connectSetter( [this](float offsetx)
            {
                mActionManager.transformActions().setEntityBaseLineOffset(mEntity, { offsetx, mEntity.get<ungod::TransformComponent>().getBaseLineOffsets().y });
            } );
            mBaseLineOffsetX->connectGetter( [this]()
            {
                return mEntity.get<ungod::TransformComponent>().getBaseLineOffsets().x;
            } );
            boxsizer->Add(mBaseLineOffsetX,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mBaseLineOffsetY = new StatDisplay<float>("base line offset right:", this, -1);
            mBaseLineOffsetY->connectSetter( [this](float offsety)
            {
                mActionManager.transformActions().setEntityBaseLineOffset(mEntity, { mEntity.get<ungod::TransformComponent>().getBaseLineOffsets().x, offsety });
            } );
            mBaseLineOffsetY->connectGetter( [this]()
            {
                return mEntity.get<ungod::TransformComponent>().getBaseLineOffsets().y;
            } );
            boxsizer->Add(mBaseLineOffsetY,0,wxALIGN_CENTER_HORIZONTAL);
        }

        refreshStats();

        SetSizer(boxsizer);
        Fit();
    }

    void EntityTransformWindow::onEntityContentsChanged(ungod::Entity e)
    {

    }

    void EntityTransformWindow::onEntityPositionChanged(ungod::Entity e, const sf::Vector2f& pos)
    {
        refreshStats();
    }

    void EntityTransformWindow::onEntitySizeChanged(ungod::Entity e, const sf::Vector2f& s)
    {
        refreshStats();
    }

    void EntityTransformWindow::refreshStats()
    {
        mPosX->setValue(mEntity.modify<ungod::TransformComponent>().getPosition().x);
        mPosY->setValue(mEntity.modify<ungod::TransformComponent>().getPosition().y);
        mScaleX->setValue(mEntity.modify<ungod::TransformComponent>().getScale().x);
        mScaleY->setValue(mEntity.modify<ungod::TransformComponent>().getScale().y);
        mBaseLineOffsetX->setValue(mEntity.modify<ungod::TransformComponent>().getBaseLineOffsets().x);
        mBaseLineOffsetY->setValue(mEntity.modify<ungod::TransformComponent>().getBaseLineOffsets().y);
    }
}

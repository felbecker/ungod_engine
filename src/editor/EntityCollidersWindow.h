#ifndef UEDIT_ENTITY_COLLIDERS_WINDOW_H
#define UEDIT_ENTITY_COLLIDERS_WINDOW_H

#include <wx/window.h>
#include <wx/sizer.h>
#include <wx/choice.h>
#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/button.h>
#include "WorldActionWrapper.h"
#include "StatDisplayer.h"

namespace uedit
{
    template<std::size_t CONTEXT>
    class EntityCollidersWindow : public wxWindow
    {
    public:
        EntityCollidersWindow(ungod::Entity e, WorldActionWrapper& waw, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz);


        void onColliderChanged(ungod::Entity e);

        void displayRigidbody(int index);

        void updateRigidbodyList();

    private:
        ungod::Entity mEntity;
        WorldActionWrapper& mWaw;
        int mSelection;
        wxChoice* mRigidbodyChoice;
        wxListCtrl* mRigidbodies;
        StatDisplay<float>* mUpLeftX;
        StatDisplay<float>* mUpLeftY;
        StatDisplay<float>* mDownRightX;
        StatDisplay<float>* mDownRightY;
        StatDisplay<float>* mRotation;

    private:
        void onRigidbodyListSelect(wxListEvent& event);
        void onColliderAddClicked(wxCommandEvent& event);
        void onColliderRemoveClicked(wxCommandEvent& event);
    };



    template<std::size_t CONTEXT>
    EntityCollidersWindow<CONTEXT>::EntityCollidersWindow(ungod::Entity e, WorldActionWrapper& waw, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz)
        : wxWindow(parent, id, pos, siz), mEntity(e), mWaw(waw), mSelection(0)
    {
        wxBoxSizer* boxsizer = new wxBoxSizer(wxVERTICAL);

        unsigned num = mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders().size();

        mRigidbodies = new wxListCtrl(this, -1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
        wxListItem bodyID;
        bodyID.SetId(0);
        bodyID.SetText( _("ID") );
        bodyID.SetWidth(60);
        mRigidbodies->InsertColumn(0, bodyID);
        mRigidbodies->Bind(wxEVT_LIST_ITEM_ACTIVATED, &EntityCollidersWindow<CONTEXT>::onRigidbodyListSelect, this);
        boxsizer->Add(mRigidbodies,2,wxALIGN_CENTER_HORIZONTAL);


        {
            wxBoxSizer* buttonsizer = new wxBoxSizer(wxHORIZONTAL);

            wxButton* addButton = new wxButton(this, -1, _("add Collider"));
            addButton->Bind(wxEVT_BUTTON, &EntityCollidersWindow<CONTEXT>::onColliderAddClicked, this);
            buttonsizer->Add(addButton,0,wxALIGN_CENTER_HORIZONTAL);

            wxButton* removeButton = new wxButton(this, -1, _("remove Collider"));
            removeButton->Bind(wxEVT_BUTTON, &EntityCollidersWindow<CONTEXT>::onColliderRemoveClicked, this);
            buttonsizer->Add(removeButton,0,wxALIGN_CENTER_HORIZONTAL);

            boxsizer->Add(buttonsizer,1,wxALIGN_CENTER_HORIZONTAL);
        }


        {
            mUpLeftX = new StatDisplay<float>("up left x:", this, -1);
            boxsizer->Add(mUpLeftX,1,wxALIGN_CENTER_HORIZONTAL);
        }
        {
            mUpLeftY = new StatDisplay<float>("up left y:", this, -1);
            boxsizer->Add(mUpLeftY,1,wxALIGN_CENTER_HORIZONTAL);
        }
        {
            mDownRightX = new StatDisplay<float>("down right x:", this, -1);
            boxsizer->Add(mDownRightX,1,wxALIGN_CENTER_HORIZONTAL);
        }
        {
            mDownRightY = new StatDisplay<float>("down right y:", this, -1);
            boxsizer->Add(mDownRightY,1,wxALIGN_CENTER_HORIZONTAL);
        }
        {
            mRotation = new StatDisplay<float>("rotation:", this, -1);
            boxsizer->Add(mRotation,1,wxALIGN_CENTER_HORIZONTAL);
        }

        if (num > 0)
            displayRigidbody(0);
        else
            displayRigidbody(-1);

        SetSizer(boxsizer);
        Fit();

        updateRigidbodyList();
        if (e.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders().size() > 0 )
            displayRigidbody(0);
        else
            displayRigidbody(-1);
    }


    template<std::size_t CONTEXT>
    void EntityCollidersWindow<CONTEXT>::onColliderChanged(ungod::Entity e)
    {
        updateRigidbodyList();
        if (mSelection != -1 && (unsigned)mSelection < e.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders().size())
            displayRigidbody(mSelection);
        else
            displayRigidbody(-1);
    }


    template<std::size_t CONTEXT>
    void EntityCollidersWindow<CONTEXT>::displayRigidbody(int index)
    {
        mSelection = index;
        if (mSelection == -1) //no rigidbody selected
        {
            {
                mUpLeftX->connectSetter( [this, index](float pos)
                        {
                        } );
                mUpLeftX->connectGetter( [this, index]()
                        {
                            return 0.0f;
                        } );
                mUpLeftX->setValue(0.0f);
                mUpLeftX->Hide();
            }
            {
                mUpLeftY->connectSetter( [this, index](float pos)
                        {
                        } );
                mUpLeftY->connectGetter( [this, index]()
                        {
                            return 0.0f;
                        } );
                mUpLeftY->setValue(0.0f);
                mUpLeftY->Hide();
            }
            {
                mDownRightX->connectSetter( [this, index](float pos)
                        {
                        } );
                mDownRightX->connectGetter( [this, index]()
                        {
                            return 0.0f;
                        } );
                mDownRightX->setValue(0.0f);
                mDownRightX->Hide();
            }
            {
                mDownRightY->connectSetter( [this, index](float pos)
                        {
                        } );
                mDownRightY->connectGetter( [this, index]()
                        {
                            return 0.0f;
                        } );
                mDownRightY->setValue(0.0f);
                mDownRightY->Hide();
            }
            {
                mRotation->connectSetter( [this, index](float rot)
                        {
                        } );
                mRotation->connectGetter( [this, index]()
                        {
                            return 0.0f;
                        } );
                mRotation->setValue(0.0f);
                mRotation->Hide();
            }
        }
        else
        {
            {
                mUpLeftX->connectSetter( [this, index](float pos)
                        {
                            sf::Vector2f upleft = mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders()[index].getUpleft();
                            upleft.x = pos;
                            mWaw.setColliderUpLeft(mEntity, index, upleft);
                        } );
                mUpLeftX->connectGetter( [this, index]()
                        {
                            return mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders()[index].getUpleft().x;
                        } );
                mUpLeftX->setValue(mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders()[index].getUpleft().x);
                mUpLeftX->Show();
            }
            {
                mUpLeftY->connectSetter( [this, index](float pos)
                        {
                            sf::Vector2f upleft = mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders()[index].getUpleft();
                            upleft.y = pos;
                            mWaw.setColliderUpLeft(mEntity, index, upleft);
                        } );
                mUpLeftY->connectGetter( [this, index]()
                        {
                            return mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders()[index].getUpleft().y;
                        } );
                mUpLeftY->setValue(mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders()[index].getUpleft().y);
                mUpLeftY->Show();
            }
            {
                mDownRightX->connectSetter( [this, index](float pos)
                        {
                            sf::Vector2f downright = mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders()[index].getDownright();
                            downright.x = pos;
                            mWaw.setColliderDownRight(mEntity, index, downright);
                        } );
                mDownRightX->connectGetter( [this, index]()
                        {
                            return mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders()[index].getDownright().x;
                        } );
                mDownRightX->setValue(mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders()[index].getDownright().x);
                mDownRightX->Show();
            }
            {
                mDownRightY->connectSetter( [this, index](float pos)
                        {
                            sf::Vector2f downright = mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders()[index].getDownright();
                            downright.y = pos;
                            mWaw.setColliderDownRight(mEntity, index, downright);
                        } );
                mDownRightY->connectGetter( [this, index]()
                        {
                            return mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders()[index].getDownright().y;
                        } );
                mDownRightY->setValue(mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders()[index].getDownright().y);
                mDownRightY->Show();
            }
            {
                mRotation->connectSetter( [this, index](float rot)
                        {
                            mWaw.setColliderRotation(mEntity, index, rot);
                        } );
                mRotation->connectGetter( [this, index]()
                        {
                            return mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders()[index].getRotation();
                        } );
                mRotation->setValue(mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders()[index].getRotation());
                mRotation->Show();
            }
        }


        Fit();
    }


    template<std::size_t CONTEXT>
    void EntityCollidersWindow<CONTEXT>::updateRigidbodyList()
    {
        mRigidbodies->DeleteAllItems();
        for (unsigned i = 0; i < mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders().size(); i++)
        {
            wxListItem rigidbodyItem;
            rigidbodyItem.SetId(i);
            mRigidbodies->InsertItem( rigidbodyItem );
            mRigidbodies->SetItem(i, 0, std::to_string(i));
        }
    }


    template<std::size_t CONTEXT>
    void EntityCollidersWindow<CONTEXT>::onRigidbodyListSelect(wxListEvent& event)
    {
        displayRigidbody(event.GetIndex());
    }


    template<std::size_t CONTEXT>
    void EntityCollidersWindow<CONTEXT>::onColliderAddClicked(wxCommandEvent& event)
    {
        mWaw.addCollider<CONTEXT>(mEntity, {0,0}, {50,50}, 0);
    }


    template<std::size_t CONTEXT>
    void EntityCollidersWindow<CONTEXT>::onColliderRemoveClicked(wxCommandEvent& event)
    {
        if (mSelection != -1)
        {
            mWaw.removeCollider<CONTEXT>(mEntity, mSelection);
        }
    }
}

#endif // UEDIT_ENTITY_COLLIDERS_WINDOW_H

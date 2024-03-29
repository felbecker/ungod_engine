#include "WorldGraphState.h"
#include "Utility.h"
#include "Canvas.h"
#include "EditorFrame.h"
#include <numeric>

namespace uedit
{
	const sf::Color WorldGraphState::NODE_DEFAULT_COLOR = sf::Color{ 112,128,144 };

	WorldGraphState::WorldGraphState(ungod::Application& app, ungod::StateID id, EditorState& editorState, EditorFrame* editorframe) :
		ungod::State(app, id), mEditorframe(editorframe), mEditorState(editorState), mCamera(app.getWindow()), mCamContrl(mCamera), mFont("data/arial.ttf"),
		mSelectedNode(nullptr), mClicked(false), mMouseLastPos(sf::Mouse::getPosition(app.getWindow())), mConnect(nullptr), mCornerSelected(-1),
		mNodeChangeDialog(new NodeChangeDialog(editorframe, -1, wxPoint(100, 100)))
	{
		mClickChecker.onClick([this](const sf::Vector2i& pos)
			{
				sf::Vector2f mouseWorldPos = mApp.getWindow().mapPixelToCoords(pos, mCamera.getView())/SCALE;
				mEditorState.getWorldGraph().updateReferencePosition(mouseWorldPos);
				if (mEditorState.getWorldGraph().getActiveNode())
				{
					mouseWorldPos = mouseWorldPos - mEditorState.getWorldGraph().getActiveNode()->getPosition();
					mEditorState.getWorldGraph().getCamera().lookAt(mouseWorldPos);
				}
				waitAll();
				closeState();
				mApp.getStateManager().moveToForeground(EDITOR_STATE);
			});
		mNodeChangeDialog->Hide();
	}


	void WorldGraphState::handleEvent(const sf::Event& event)
	{
		mCamContrl.handleEvent(event);
		mClickChecker.handleEvent(event);

		if (event.type == sf::Event::MouseButtonPressed)
		{
			sf::Vector2i mousePos = sf::Mouse::getPosition(mApp.getWindow());
			sf::Vector2f mouseWorldPos = mApp.getWindow().mapPixelToCoords(mousePos, mCamera.getView()) / SCALE;
			ungod::WorldGraphNode* clickedNode = mEditorState.getWorldGraph().getNode(mouseWorldPos);
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				if (!mClicked)
				{
					mTotalMove = { 0.0f,0.0f };
					if (clickedNode)
					{
						auto bounds = clickedNode->getBounds();
						std::array<sf::Vector2f, 4> points{ sf::Vector2f{bounds.left, bounds.top},
															sf::Vector2f{bounds.left+bounds.width, bounds.top},
															sf::Vector2f{bounds.left + bounds.width, bounds.top + bounds.height},
															sf::Vector2f{bounds.left, bounds.top + bounds.height} };
						for (int i = 0; i < 4; i++)
							if (ungod::distance(points[i], mouseWorldPos) < CORNER_CLICK_RANGE)
							{
								mCornerSelected = i;
								break;
							}
					}
					mSelectedNode = clickedNode;
				}
				mClicked = true;
			}
			else if (event.mouseButton.button == sf::Mouse::Right)
			{
				wxMenu mnu;
				if (clickedNode == mConnect)
					mnu.Append(0, "Untrack");
				else
					mnu.Append(0, "Track");
                if (mConnect && mConnect != clickedNode)
                {
					if (mEditorState.getWorldGraph().areConnected(*mConnect, *clickedNode))
						mnu.Append(1, "Disconnect with tracked");
					else
						mnu.Append(1, "Connect with tracked");
                }
                mnu.Append(2, "Change properties");
                mnu.Append(3, "Set color");
				mnu.Bind(wxEVT_COMMAND_MENU_SELECTED, [this, clickedNode](wxCommandEvent& event)
					{
						switch (event.GetId()) {
						case 0:
						{
							if (mConnect == clickedNode)
								mConnect = nullptr;
							else
								mConnect = clickedNode;
							break;
						}
						case 1:
						{
							if (mEditorState.getWorldGraph().areConnected(*mConnect, *clickedNode))
								mEditorState.getWorldGraph().disconnect(*mConnect, *clickedNode);
							else
								mEditorState.getWorldGraph().connect(*mConnect, *clickedNode);
							waitAll();
							break;
						}
                        case 2:
						{
							mNodeChangeDialog->setNode(clickedNode);
							mNodeChangeDialog->Show();
							break;
						}
                        case 3:
						{
							sf::Color prev = NODE_DEFAULT_COLOR;
							auto res = mColorMapper.find(clickedNode);
							if (res != mColorMapper.end())
								prev = res->second;
							mColorData.SetColour(convertColor(prev));
							wxColourDialog dlg(mEditorframe->getCanvas(), &mColorData);
							dlg.Bind(wxEVT_COLOUR_CHANGED, [this, clickedNode](wxColourDialogEvent& event)
							{
								mColorMapper[clickedNode] = convertColor(event.GetColour());
							});
							if (dlg.ShowModal() != wxID_OK)
								mColorMapper[clickedNode] = prev;
							break;
						}
						default:
							break;
						}
					});
				mEditorframe->PopupMenu(&mnu);
			}
		}
		else if (event.type == sf::Event::MouseMoved)
		{
			if (mSelectedNode)
			{
				sf::Vector2f mouseMove = (1 / SCALE) * mCamera.getZoom() * sf::Vector2f{ (float)(event.mouseMove.x - mMouseLastPos.x), (float)(event.mouseMove.y - mMouseLastPos.y) };
				if (mCornerSelected == -1)
					mSelectedNode->move(mouseMove);
				mTotalMove += mouseMove;
			}
			mMouseLastPos.x = event.mouseMove.x;
			mMouseLastPos.y = event.mouseMove.y;
		}
		else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
		{
			switch (mCornerSelected)
			{
			case 0:
			{
				mSelectedNode->extend(-mTotalMove, {});

				//mSelectedNode->move(mTotalMove);
				//mSelectedNode->setSize(sf::Vector2f{ mSelectedNode->getBounds().width, mSelectedNode->getBounds().height }-mTotalMove);
				break;
			}
			case 1:
			{
				mSelectedNode->extend({ 0.0f, -mTotalMove.y }, { mTotalMove.x , 0.0f });

				//mSelectedNode->move({ 0.0f, mTotalMove.y });
				//mSelectedNode->setSize(sf::Vector2f{ mSelectedNode->getBounds().width, mSelectedNode->getBounds().height }+sf::Vector2f{ mTotalMove.x, -mTotalMove.y });
				break;
			}
			case 2:
			{
				mSelectedNode->extend({}, mTotalMove);
				//mSelectedNode->setSize(sf::Vector2f{ mSelectedNode->getBounds().width, mSelectedNode->getBounds().height }+mTotalMove);
				break;
			}
			case 3:
			{
				mSelectedNode->extend({ -mTotalMove.x , 0.0f } , { 0.0f, mTotalMove.y });

				//mSelectedNode->move({ mTotalMove.x, 0.0f });
				//mSelectedNode->setSize(sf::Vector2f{ mSelectedNode->getBounds().width, mSelectedNode->getBounds().height }+sf::Vector2f{ -mTotalMove.x, mTotalMove.y });
				break;
			}
			default:
				break;
			}
			mClicked = false;
			mSelectedNode = nullptr;
			mCornerSelected = -1;
		}
	}

	void WorldGraphState::update(float delta)
	{
	}

	void WorldGraphState::render(sf::RenderTarget& target, sf::RenderStates states)
	{
		//states.transform.scale(SCALE, SCALE);
		mCamera.renderBegin();
		std::list<int> prioritySorted(mEditorState.getWorldGraph().getALlists().getVertexCount());
		std::iota(prioritySorted.begin(), prioritySorted.end(), 0);
		auto sortF = [this] (int i, int j) 
			{ return mEditorState.getWorldGraph().getNode(i)->getPriority() < mEditorState.getWorldGraph().getNode(j)->getPriority(); };
		prioritySorted.sort(sortF);
		for (unsigned i : prioritySorted)
		{
			ungod::WorldGraphNode* node = mEditorState.getWorldGraph().getNode(i);
			sf::FloatRect bounds = node->getBounds();

			//draw rect
			sf::Color rectColor = NODE_DEFAULT_COLOR;
			auto res = mColorMapper.find(node);
			if (res != mColorMapper.end())
				rectColor = res->second;

			sf::RectangleShape rect{ {SCALE * bounds.width, SCALE * bounds.height} };
			rect.setFillColor(rectColor);
			rect.setOutlineThickness(100*SCALE);
			if (node == mConnect)
				rect.setOutlineColor(sf::Color::Red);
			else
				rect.setOutlineColor(sf::Color::White);
			rect.setPosition({ SCALE*bounds.left, SCALE * bounds.top});
			target.draw(rect, states);

			//draw incident edges
			for (const auto& n : mEditorState.getWorldGraph().getALlists().getNeighbors(i))
			{
				sf::FloatRect adjacentBounds = mEditorState.getWorldGraph().getNode(n.destination)->getBounds();
				sf::Vertex line[2];
				line[0].position = sf::Vector2f{ SCALE * (bounds.left+bounds.width/2), SCALE * (bounds.top + bounds.height / 2) };
				line[0].color = sf::Color::White;
				line[1].position = sf::Vector2f{ SCALE * (adjacentBounds.left + adjacentBounds.width / 2), SCALE * (adjacentBounds.top + adjacentBounds.height / 2) };
				line[1].color = sf::Color::White;
				target.draw(line, 2, sf::LineStrip, states);
			}

			//draw text
			if (mFont.isLoaded())
			{
				float resizeFactor = (rect.getSize().x + rect.getSize().y) / 2000.0f;
				unsigned textSize = (unsigned)std::ceil(TEXTSIZE * resizeFactor / mCamera.getZoom());
				textSize = std::max(std::min(textSize, 80u), 5u);

				std::string id = node->getIdentifier();
				if (node == mEditorState.getWorldGraph().getActiveNode())
					id += " (active)";
				sf::Text textID{ id, mFont.get(), textSize };
                textID.setPosition(SCALE * (bounds.left + 0.25f*bounds.width), SCALE * (bounds.top + 0.25f*bounds.height));
				textID.setScale(mCamera.getZoom(), mCamera.getZoom());
                
                std::string posStr = "(" + std::to_string(node->getBounds().left) + "," +
                    std::to_string(node->getBounds().top) + ")";
                sf::Text textNodepos{ posStr, mFont.get(), (unsigned)(textSize/2) };
                textNodepos.setPosition(SCALE * (bounds.left + 0.25f * bounds.width), SCALE * (bounds.top + 0.3f * bounds.height) + textID.getGlobalBounds().height);
				textNodepos.setScale(mCamera.getZoom(), mCamera.getZoom());

                std::string sizeStr = "(" + std::to_string(node->getBounds().width) + "," +
                    std::to_string(node->getBounds().height) + ")";
                sf::Text textNodesize{ sizeStr, mFont.get(), (unsigned)(textSize / 2) };
                textNodesize.setPosition(SCALE * (bounds.left + 0.25f * bounds.width), SCALE * (bounds.top + 0.3f * bounds.height) + textID.getGlobalBounds().height + textNodepos.getGlobalBounds().height);
				textNodesize.setScale(mCamera.getZoom(), mCamera.getZoom());

                target.draw(textID, states);
                target.draw(textNodepos, states);
                target.draw(textNodesize, states);

				if (node->isLoaded())
				{
					sf::Text textLoaded{ "[loaded]", mFont.get(), (unsigned)(textSize / 2) };
					textLoaded.setPosition(SCALE * (bounds.left + 0.25f * bounds.width), SCALE * (bounds.top + 0.4f * bounds.height) + textID.getGlobalBounds().height + 2*textNodepos.getGlobalBounds().height);
					textLoaded.setScale(mCamera.getZoom(), mCamera.getZoom());
					target.draw(textLoaded, states);
				}
			}
		}
		mCamera.renderEnd();
	}

	void WorldGraphState::init()
	{
		mClicked = false;
		mSelectedNode = nullptr;
        mCamera.setZoom(0.25f);
	}

	void WorldGraphState::close()
	{

	}

	void WorldGraphState::waitAll()
	{
		for (unsigned i = 0; i < mEditorState.getWorldGraph().getALlists().getVertexCount(); i++)
		{
			mEditorState.getWorldGraph().getNode(i)->wait();
		}
	}

	NodeChangeDialog::NodeChangeDialog(wxWindow* parent, wxWindowID id, const wxPoint& pos)
		: wxDialog(parent, id, _("_ properties"), pos, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP), mNode(nullptr)
	{
		wxSizer* boxsizer = new wxBoxSizer(wxVERTICAL);

		{
			mPosX = new StatDisplay<float>("position x:", this, -1);
			mPosX->connectSetter([this](float x)
				{
					if (!mNode) return;
					mNode->setPosition({ x, mNode->getPosition().y });
				});
			mPosX->connectGetter([this]() 
				{
					if (!mNode) return 0.0f;
					return mNode->getPosition().x;
				});
			boxsizer->Add(mPosX, 0, wxALIGN_CENTER_HORIZONTAL);
		}
		{
			mPosY = new StatDisplay<float>("position y:", this, -1);
			mPosY->connectSetter([this](float y)
				{
					if (!mNode) return;
					mNode->setPosition({ mNode->getPosition().x, y });
				});
			mPosY->connectGetter([this]()
				{
					if (!mNode) return 0.0f;
					return mNode->getPosition().y;
				});
			boxsizer->Add(mPosY, 0, wxALIGN_CENTER_HORIZONTAL);
		}
		{
			mSizeX = new StatDisplay<float>("size x:", this, -1);
			mSizeX->connectSetter([this](float sx)
				{
					if (!mNode) return;
					if (sx > 0.0f)
					{
						bool wasActive = mNode == mNode->getGraph().getActiveNode();
						mNode->setSize({ sx, mNode->getSize().y });
						if (wasActive) updateGraphRefPos();
					}
					else
						wxMessageBox(wxT("Width has to be greater zero!"), wxT("Invalid width"), wxICON_ERROR);
				});
			mSizeX->connectGetter([this]() 
				{
					if (!mNode) return 0.0f;
					return mNode->getSize().x;
				});
			boxsizer->Add(mSizeX, 0, wxALIGN_CENTER_HORIZONTAL);
		}
		{
			mSizeY = new StatDisplay<float>("size y:", this, -1);
			mSizeY->connectSetter([this](float sy)
				{
					if (!mNode) return;
					if (sy > 0.0f)
					{
						bool wasActive = mNode == mNode->getGraph().getActiveNode();
						mNode->setSize({ mNode->getSize().x, sy });
						if (wasActive) updateGraphRefPos();
					}
					else
						wxMessageBox(wxT("Height has to be greater zero!"), wxT("Invalid height"), wxICON_ERROR);
				});
			mSizeY->connectGetter([this]() 
				{
					if (!mNode) return 0.0f;
					return mNode->getSize().y;
				});
			boxsizer->Add(mSizeY, 0, wxALIGN_CENTER_HORIZONTAL);
		}
		{
			mIdentifier = new StatDisplay<std::string>("identifier:", this, -1);
			mIdentifier->connectSetter([this](std::string id)
				{
					if (!mNode) return;
					SetTitle(_(id + " properties"));
					mNode->setIdentifier(id);
				});
			mIdentifier->connectGetter([this]() -> std::string
				{
					if (!mNode) return "";
					return mNode->getIdentifier();
				});
			boxsizer->Add(mIdentifier, 0, wxALIGN_CENTER_HORIZONTAL);
		}
		{
			mPriority = new StatDisplay<int>("priority:", this, -1);
			mPriority->connectSetter([this](int priority)
				{
					if (!mNode) return;
					mNode->setPriority(priority);
				});
			mPriority->connectGetter([this]()
				{
					if (!mNode) return 0;
					return mNode->getPriority();
				});
			boxsizer->Add(mPriority, 0, wxALIGN_CENTER_HORIZONTAL);
		}
		{
			mTopExtend = new wxTextCtrl(this, -1, _("0"));
			boxsizer->Add(mTopExtend, 0, wxALIGN_CENTER_HORIZONTAL);
			{
				wxSizer* hsizer = new wxBoxSizer(wxHORIZONTAL);
				mLeftExtend = new wxTextCtrl(this, -1, _("0"));
				mRightExtend = new wxTextCtrl(this, -1, _("0"));
				hsizer->Add(mLeftExtend, 0, wxALIGN_CENTER_VERTICAL);
				hsizer->Add(mRightExtend, 0, wxALIGN_CENTER_VERTICAL);
				boxsizer->Add(hsizer, 0, wxALIGN_CENTER_HORIZONTAL);
			}
			mBottomExtend = new wxTextCtrl(this, -1, _("0"));
			boxsizer->Add(mBottomExtend, 0, wxALIGN_CENTER_HORIZONTAL);
			wxButton* extendButton = new wxButton(this, -1, _("extend"));
			extendButton->Bind(wxEVT_BUTTON, &NodeChangeDialog::onExtendButtonClicked, this);
			boxsizer->Add(extendButton, 0, wxALIGN_CENTER_HORIZONTAL);
		}

		SetSizer(boxsizer);
		Fit();
	}

	void NodeChangeDialog::setNode(ungod::WorldGraphNode* node)
	{
		mNode = node;
		SetTitle(_(node->getIdentifier() + " properties"));
		mNodeChangedLink.disconnect();
		mNodeChangedLink = node->onNodeChanged([this]() { refresh(); });
		refresh();
		mIdentifier->refreshValue();
	}

	void NodeChangeDialog::refresh()
	{
		mPosX->refreshValue();
		mPosY->refreshValue();
		mSizeX->refreshValue();
		mSizeY->refreshValue();
		mPriority->refreshValue();
	}

	void NodeChangeDialog::updateGraphRefPos()
	{
		//if node is active, updating its size may cause and oob ref position
		//we reset the ref position to the new node center to make sure it is still inside the node
		sf::Vector2f newRef = mNode->getPosition() + 0.5f*mNode->getSize();
		mNode->getGraph().updateReferencePosition(newRef);
	}

	void NodeChangeDialog::onExtendButtonClicked(wxCommandEvent& event)
	{
		try
		{
			float left = std::stof(std::string(mLeftExtend->GetValue().mb_str()));
			float top = std::stof(std::string(mTopExtend->GetValue().mb_str()));
			float right = std::stof(std::string(mRightExtend->GetValue().mb_str()));
			float bottom = std::stof(std::string(mBottomExtend->GetValue().mb_str()));
			mNode->extend({ left, top }, { right, bottom });
		}
		catch (const std::exception&)
		{
			auto err = wxMessageDialog(this, _("At least one of the extension text fields does not contain a valid number."));
			err.ShowModal();
		}
	}
}

namespace ungod
{
	//serialization code
	void SerialBehavior<uedit::WorldGraphState>::serialize(const uedit::WorldGraphState& data, MetaNode serializer, SerializationContext& context)
	{
		std::vector<sf::Color> colors;
		for (unsigned i = 0; i < data.mEditorState.getWorldGraph().getALlists().getVertexCount(); i++)
		{
			ungod::WorldGraphNode* node = data.mEditorState.getWorldGraph().getNode(i);
			auto res = data.mColorMapper.find(node);
			if (res != data.mColorMapper.end())
				colors.emplace_back(res->second);
			else
				colors.emplace_back(uedit::WorldGraphState::NODE_DEFAULT_COLOR);
		}
		context.serializePropertyContainer("nodeColorMap", colors, serializer);
	}

	void DeserialBehavior<uedit::WorldGraphState>::deserialize(uedit::WorldGraphState& data, MetaNode deserializer, DeserializationContext& context)
	{
		std::vector<sf::Color> colors;
		auto init = [&colors](std::size_t num) { colors.reserve(num); };
		auto ref = [&colors](const sf::Color& col) { colors.emplace_back(col); };
		context.first(context.deserializeContainer<sf::Color>(init, ref), "nodeColorMap", deserializer);
		for (int i = 0; i < colors.size(); i++)
		{
			ungod::WorldGraphNode* node = data.mEditorState.getWorldGraph().getNode(i);
			if (node)
				data.mColorMapper[node] = colors[i];
		}
		data.waitAll();
	}
}
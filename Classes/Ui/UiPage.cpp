#include "UiPage.h"
#include "cocostudio/ActionTimeline/CSLoader.h"
#include "ui/UIHelper.h"

USING_NS_CC;

using namespace ui;

UiPage::UiPage(ModelViewer& viewer)
	: _viewer(viewer)
	, _MsgToUserAlpha(1.f)
	, _addState(AddingState::None)
{
}

UiPage::~UiPage()
{

}

bool UiPage::init()
{
	Layer::init();

	Widget* hud = static_cast<Widget*>(CSLoader::createNode("ModelViewSelect.csb"));

	if (hud != nullptr)
	{
		hud->setCameraMask((unsigned short)CameraFlag::DEFAULT);
		addChild(hud);

		_titleLabel = static_cast<ui::Text*>(hud->getChildByName("global_anchor_lt")->getChildByName("title"));
		_modelLabel = static_cast<ui::Text*>(_titleLabel->getChildByName("modelName"));
		_animLabel = static_cast<ui::Text*>(_modelLabel->getChildByName("animName"));

		//////////////////////////////////////////////////////////////////////////
		_addModelBtn = static_cast<ui::Button*>(ui::Helper::seekWidgetByName(hud, "AddModelBtn"));
		_addModelBtn->addClickEventListener(CC_CALLBACK_1(UiPage::showAdding, this,AddingState::AddModel));
		_modelListView = static_cast<ui::ListView*>(hud->getChildByName("global_anchor_rt")->getChildByName("modelListView"));
		_modelListView->setClippingEnabled(true);
		_modelListView->setTouchEnabled(true);
		_modelListView->setSwallowTouches(true);
		_modelListView->addEventListener((ui::ListView::ccListViewCallback)CC_CALLBACK_2(UiPage::selectedModelEvent, this));

		_addAnimBtn = static_cast<ui::Button*>(ui::Helper::seekWidgetByName(hud, "AddAnimBtn"));
		_addAnimBtn->addClickEventListener(CC_CALLBACK_1(UiPage::showAdding, this,AddingState::AddAnim));
		_animListView = static_cast<ui::ListView*>(hud->getChildByName("global_anchor_rt")->getChildByName("animListView"));
		_animListView->setClippingEnabled(true);
		_animListView->setTouchEnabled(true);
		_animListView->setSwallowTouches(true);
		_animListView->addEventListener((ui::ListView::ccListViewCallback)CC_CALLBACK_2(UiPage::selectedAnimEvent, this));

		_DelModelBtn = static_cast<ui::Button*>(ui::Helper::seekWidgetByName(hud, "DelModel"));
		_DelModelBtn->addClickEventListener(CC_CALLBACK_1(UiPage::deleteModel, this));

		_DelAnimBtn = static_cast<ui::Button*>(ui::Helper::seekWidgetByName(hud, "DelAnim"));
		_DelAnimBtn->addClickEventListener(CC_CALLBACK_1(UiPage::deleteAnim, this));
		//////////////////////////////////////////////////////////////////////////

		_addDialogImage = ui::Helper::seekWidgetByName(hud, "addDialog");		
		_addDialogImage->setVisible(false);
		_addFileName = static_cast<ui::TextField*>(ui::Helper::seekWidgetByName(hud, "addNameText"));
		_addOkBtn = static_cast<ui::Button*>(ui::Helper::seekWidgetByName(hud, "addGO"));
		_addOkBtn->addClickEventListener(CC_CALLBACK_1(UiPage::AddNewItem, this));

		_addTexBg = ui::Helper::seekWidgetByName(hud, "addForTexBg");
		_addTexName = static_cast<ui::TextField*>(ui::Helper::seekWidgetByName(hud, "addForTex"));
		//////////////////////////////////////////////////////////////////////////

		_FromFrame  = static_cast<ui::TextField*>(ui::Helper::seekWidgetByName(hud,"FromFrame"));
		_ToFrame = static_cast<ui::TextField*>(ui::Helper::seekWidgetByName(hud,"ToFrame"));
		
		_EnsureModifyAnimBtn = static_cast<ui::Button*>(ui::Helper::seekWidgetByName(hud, "changeAnim"));
		_EnsureModifyAnimBtn->addClickEventListener(CC_CALLBACK_1(UiPage::modifyAnim, this));

		_MsgToUser = static_cast<ui::Text*>(ui::Helper::seekWidgetByName(hud, "info"));

		_SaveBtn = static_cast<ui::Button*>(ui::Helper::seekWidgetByName(hud,"saveToFile"));
		_SaveBtn->addClickEventListener(CC_CALLBACK_1(UiPage::serializeToJson, this));

		showUserMsg("Arrow key to switch Model/Animation.\nUse MOUSE to control view. Key SPACE to reset camera.");
		scheduleUpdate();
	}

	return true;
}

void UiPage::selectedModelEvent(Ref *pSender, ListView::EventType type)
{
	if (type == ListView::EventType::ON_SELECTED_ITEM_START) {
		ui::ListView* listView = static_cast<ui::ListView*>(pSender);
		int index = listView->getCurSelectedIndex();
		if ( index > 0)
			_viewer.changeViewTarget((static_cast<Button*>(listView->getItem(index)))->getTitleText());
	}
}

void UiPage::selectedAnimEvent(Ref *pSender, ui::ListView::EventType type)
{
	if (type == ListView::EventType::ON_SELECTED_ITEM_START) {
		ui::ListView* listView = static_cast<ui::ListView*>(pSender);
		int index = listView->getCurSelectedIndex();
		_viewer.changeAnim((static_cast<Button*>(listView->getItem(index)))->getTitleText());
	}
}

void UiPage::setAnimData(const std::string& animName,int from,int to)
{
	_animLabel->setString(animName);

	char text[25];
	_itoa(from, text, 10);
	_FromFrame->setString(text);

	_itoa(to, text, 10);
	_ToFrame->setString(text);
}

void UiPage::addModelToViewList(const std::string& modelName)
{
	if (_defaultBtninListView == nullptr){
		_defaultBtninListView = static_cast<ui::Button*>(_modelListView->getItem(0)->clone());
		static_cast<ui::Button*>(_modelListView->getItem(0))->setEnabled(false);
	}

	ui::Button* newWidget = static_cast<ui::Button*>(_defaultBtninListView->clone());
	newWidget->setTitleText(modelName);
	_modelListView->insertCustomItem(newWidget, 1);

}

void UiPage::addAnimToViewList(const std::string& animName)
{
	if (_defaultBtninListView == nullptr){
		_defaultBtninListView = static_cast<ui::Button*>(_modelListView->getItem(0)->clone());
		static_cast<ui::Button*>(_modelListView->getItem(0))->setEnabled(false);
	}

	ui::Button* newWidget = static_cast<ui::Button*>(_defaultBtninListView->clone());
	newWidget->setTitleText(animName);

	if (_animListView->getItems().empty())
	{
		_animListView->insertCustomItem(newWidget, 0);
	}
	else
		_animListView->insertCustomItem(newWidget, 1);

}

void UiPage::update(float t)
{
	if (_MsgToUser->isVisible())
	{
		_MsgToUserAlpha -= t*0.2f;
		_MsgToUser->setOpacity(_MsgToUserAlpha * 255);
		if (_MsgToUserAlpha < 0.f)
		{
			_MsgToUser->setVisible(false);
			_MsgToUserAlpha = 0;
		}
	}
}

void UiPage::showUserMsg(const std::string& msg,Color3B	c)
{
	_MsgToUser->setVisible(true);
	_MsgToUserAlpha = 1.f;

	_MsgToUser->setString(msg);
	_MsgToUser->setColor(c);
}

void UiPage::modifyAnim(Ref* pSender)
{
	bool bCanModify = true;
	if (_FromFrame->getString().empty() || _ToFrame->getString().empty() ){
		showUserMsg("Input error.", Color3B::RED);
		bCanModify = false;
	}

	//TODO:
// 	if (_animLabel->getString() == FileHandler::s_DefaultAnim){
// 		bCanModify = false;
// 		showUserMsg("Can't modify the default animation", Color3B::RED);
// 	}

	int newFromFrame, newToFrame;
	newFromFrame = atoi(_FromFrame->getString().c_str());
	newToFrame = atoi(_ToFrame->getString().c_str());

	if (bCanModify)	{
		showUserMsg("Animation [" + _animLabel->getString() + "] modified.", Color3B::GREEN);
		_viewer.modifyAnim(newFromFrame, newToFrame);
	}
}

void UiPage::AddNewItem(Ref* pSender)
{
	bool canAdd = true;

	_addDialogImage->setVisible(false);

	if (!_addFileName->getString().empty()){
		//Check if the name exist
		if (_addState == AddingState::AddModel)
		{
			if (!_viewer.checkTargetNameLegal(_addFileName->getString())){
				canAdd = false;
				showUserMsg("Model name conflict!", Color3B::RED);
			}
			
		}else if (_addState == AddingState::AddAnim){

			if (!_viewer.checkTargetNameLegal(_addFileName->getString())){
				canAdd = false;
				showUserMsg("Animation name conflict!", Color3B::RED);
			}
		}
	}
	else{
		canAdd = false;
		showUserMsg("Need a name!", Color3B::RED);
	}

	if (canAdd)
	{
		if (_addState == AddingState::AddAnim)
		{
			showUserMsg("Add new anim!", Color3B::GREEN);
			_viewer.addNewAnim(_addFileName->getString());
			addAnimToViewList(_addFileName->getString());
		}
		else if (_addState == AddingState::AddModel){
			if (_viewer.addViewTarget(_addFileName->getString(), _addFileName->getString(), _addTexName->getString()))
				showUserMsg("Add new model!", Color3B::GREEN);
			else
				showUserMsg("File not found, please ensure the files are in data folder!", Color3B::RED);
		}
	}

	_addFileName->setString("");
	_addState = AddingState::None;
}

void UiPage::showAdding(Ref* pSender, AddingState state)
{
	if (_addState != AddingState::None)
		return;

	if (state == AddingState::AddAnim){
		_addTexBg->setVisible(false);
		_addTexName->setVisible(false);
	}
	else if (state == AddingState::AddModel){
		_addTexBg->setVisible(true);
		_addTexName->setVisible(true);
		_addTexName->setString("");
	}

	_addState = state;
	_addDialogImage->setVisible(true);
}

void UiPage::serializeToJson(Ref* pSender)
{
	if (_viewer.saveAll())
		showUserMsg("File saved.", Color3B::GREEN);
	else
		showUserMsg("Can't save to file!!!", Color3B::RED);
}

void UiPage::deleteModel(Ref* pSender)
{
	//Delete the ViewTarget data 
	if (!_viewer.deleteViewTarget(_titleLabel->getString())){
		showUserMsg("Cannot delete");
		return;
	}
	
	//Delete from UI view list
	auto modelItems = _modelListView->getItems();
	int i = 0;
	for (; i < modelItems.size(); ++i)	{
		if (static_cast<ui::Button*>(modelItems.at(i))->getTitleText() == _titleLabel->getString()){
			break;
		}
	}
	_modelListView->removeItem(i);

	clearAnimViewList();
	showUserMsg("Model deleted.", Color3B::GREEN);
}

void UiPage::deleteAnim(Ref* pSender)
{
// 	if (_animLabel->getString() == FileHandler::s_DefaultAnim){
// 		showUserMsg("Can't delete default animation.", Color3B::RED);
// 		return;
// 	}
//	else{
		//Delete from UI view list
		auto animItems = _animListView->getItems();
		int i = 0;
		for (; i < animItems.size(); ++i)	{
			if(static_cast<ui::Button*>(animItems.at(i))->getTitleText() == _animLabel->getString()){
				break;
			}
		}
		_animListView->removeItem(i);

		//Delete from the animation data, and reset the animation show
		if(_viewer.deleteAnim(_titleLabel->getString(), _animLabel->getString()))
			showUserMsg("Animation deleted.", Color3B::GREEN);
		else
			showUserMsg("Can't delete default animation.", Color3B::RED);
//	}
}

UiPage* UiPage::create(ModelViewer& viewer)
{
	UiPage *pRet = new(std::nothrow) UiPage(viewer);
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}
#ifndef __UI_MANANGER_H__
#define __UI_MANANGER_H__

#include "cocos2d.h"
#include "ui/UIText.h"
#include "ui/UIListView.h"
#include "ui/UIButton.h"
#include "ui/UITextField.h"

enum class UiCustomEventType{
	UCE_SELECT_MODEL,
	UCE_SELECT_ANIM,
	
	UCE_MODIFY_CURRANT_ANIM,
	
	UCE_DELETE_ANIM,
	UCE_DELETE_MODEL,

	UCE_ADD_MODEL,
	UCE_ADD_ANIM,
};

class UiCustomEventData{
public:
	static std::string	sUiCustomEventName;

	UiCustomEventData(UiCustomEventType	uiType);

	UiCustomEventType		_type;
	int						_int1;
	int						_int2;
	std::string				_info;
	std::string				_info2;
};


class UiHandler : public cocos2d::Layer{
	enum class	AddingState{ 
		AS_NONE, 
		AS_ADD_MODEL, 
		AS_ADD_ANIM 
	};

CC_CONSTRUCTOR_ACCESS:
	UiHandler();
	~UiHandler();

	virtual bool init()override;
public:
	CREATE_FUNC(UiHandler);

	static UiHandler*	getInstance();

	void	setTitle(const std::string&	title)		{	_titleLabel->setString(title);	}
	void	setModelName(const std::string&	title)	{	_modelLabel->setString(title);	}
	
	void	setAnimName(const std::string& animName, int from, int to);

	void	addModelToViewList(const std::string& modelName);
	void	addAnimToViewList(const std::string& animName);

	void	clearAnimViewList()						{	_animListView->removeAllItems();	}

	void update(float t)override;

	void showUserMsg(const std::string&	msg, cocos2d::Color3B c = cocos2d::Color3B::WHITE);

protected:
	void selectedModelEvent(cocos2d::Ref *pSender, cocos2d::ui::ListView::EventType type);
	void selectedAnimEvent(cocos2d::Ref *pSender, cocos2d::ui::ListView::EventType type);

	void modifyAnim(cocos2d::Ref* pSender);
	void AddNewItem(cocos2d::Ref* pSender);
	void showAdding(cocos2d::Ref* pSender, AddingState state);
	void deleteModel(cocos2d::Ref* pSender);
	void deleteAnim(cocos2d::Ref* pSender);
	
	void serializeToJson(cocos2d::Ref* pSender);

private:

	//Info show on screen's left-top
	cocos2d::RefPtr<cocos2d::ui::Text>	_titleLabel;
	cocos2d::RefPtr<cocos2d::ui::Text>	_animLabel;
	cocos2d::RefPtr<cocos2d::ui::Text>	_modelLabel;

	//Model/Anim view and add
	AddingState		_addState;

	cocos2d::RefPtr<cocos2d::ui::ListView>	_modelListView;
	cocos2d::RefPtr<cocos2d::ui::ListView>	_animListView;
	cocos2d::RefPtr<cocos2d::ui::Button>	_addModelBtn;
	cocos2d::RefPtr<cocos2d::ui::Button>	_addAnimBtn;
	cocos2d::RefPtr<cocos2d::ui::Button>	_DelModelBtn;
	cocos2d::RefPtr<cocos2d::ui::Button>	_DelAnimBtn;

	//Add Model/Anim dialog
	cocos2d::RefPtr<cocos2d::ui::Widget>	_addDialogImage;	//Image view
	cocos2d::RefPtr<cocos2d::ui::TextField>	_addFileName;
	cocos2d::RefPtr<cocos2d::ui::Button>	_addOkBtn;
	cocos2d::RefPtr<cocos2d::ui::Widget>	_addTexBg;	
	cocos2d::RefPtr<cocos2d::ui::TextField>	_addTexName;

	//Edit animation
	cocos2d::RefPtr<cocos2d::ui::TextField>	_FromFrame;
	cocos2d::RefPtr<cocos2d::ui::TextField>	_ToFrame;
	cocos2d::RefPtr<cocos2d::ui::Button>	_EnsureModifyAnimBtn;

	//Serialize to json
	cocos2d::RefPtr<cocos2d::ui::Button>	_SaveBtn;

	//Message text
	cocos2d::RefPtr<cocos2d::ui::Text>		_MsgToUser;
	float _MsgToUserAlpha;
	
	cocos2d::RefPtr<cocos2d::ui::Button>	_defaultBtninListView;	//Dummy for clone
	static cocos2d::RefPtr<UiHandler>	sInstance;
};

#endif
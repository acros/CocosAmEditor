#ifndef __UI_MANANGER_H__
#define __UI_MANANGER_H__

#include "cocos2d.h"
#include "ui/UIText.h"
#include "ui/UIListView.h"
#include "ui/UIButton.h"
#include "ui/UITextField.h"
#include "Viewer/ModelViewer.h"

USING_NS_CC;

class UiPage : public cocos2d::Layer{
	enum class	AddingState{ 
		None, 
		AddModel, 
		AddAnim 
	};

CC_CONSTRUCTOR_ACCESS:
	UiPage(ModelViewer& viewer);
	~UiPage();

	virtual bool init()override;
public:
	static UiPage*	create(ModelViewer& viewer);

	void	setFriendlyName(const std::string&	title)		{	_titleLabel->setString(title);	}
	void	setModelName(const std::string&	title)	{	_modelLabel->setString(title);	}
	
	void	setAnimData(const std::string& animName, int from, int to);

	void	addModelToViewList(const std::string& modelName);
	void	addAnimToViewList(const std::string& animName);

	void	clearAnimViewList()						{	_animListView->removeAllItems();	}

	void update(float t)override;

	void showUserMsg(const std::string&	msg, Color3B c = Color3B::WHITE);

protected:
	void selectedModelEvent(Ref *pSender,ui::ListView::EventType type);
	void selectedAnimEvent(Ref *pSender, ui::ListView::EventType type);

	void modifyAnim(Ref* pSender);
	void AddNewItem(Ref* pSender);
	void showAdding(Ref* pSender, AddingState state);
	void deleteModel(Ref* pSender);
	void deleteAnim(Ref* pSender);
	
	void serializeToJson(Ref* pSender);

private:
	ModelViewer& _viewer;

	//Info show on screen's left-top
	RefPtr<ui::Text>	_titleLabel;
	RefPtr<ui::Text>	_animLabel;
	RefPtr<ui::Text>	_modelLabel;

	//Model/Anim view and add
	AddingState		_addState;

	RefPtr<ui::ListView>	_modelListView;
	RefPtr<ui::ListView>	_animListView;
	RefPtr<ui::Button>	_addModelBtn;
	RefPtr<ui::Button>	_addAnimBtn;
	RefPtr<ui::Button>	_DelModelBtn;
	RefPtr<ui::Button>	_DelAnimBtn;

	//Add Model/Anim dialog
	RefPtr<ui::Widget>	_addDialogImage;	//Image view
	RefPtr<ui::TextField>	_addFileName;
	RefPtr<ui::Button>	_addOkBtn;
	RefPtr<ui::Widget>	_addTexBg;	
	RefPtr<ui::TextField>	_addTexName;

	//Edit animation
	RefPtr<ui::TextField>	_FromFrame;
	RefPtr<ui::TextField>	_ToFrame;
	RefPtr<ui::Button>	_EnsureModifyAnimBtn;

	//Serialize to json
	RefPtr<ui::Button>	_SaveBtn;

	//Message text
	RefPtr<ui::Text>		_MsgToUser;
	float _MsgToUserAlpha;
	
	RefPtr<ui::Button>	_defaultBtninListView;	//Dummy for clone
};

#endif
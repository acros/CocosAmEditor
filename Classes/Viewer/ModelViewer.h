#ifndef __MODELVIEWER_SCENE_H__
#define __MODELVIEWER_SCENE_H__

#include "cocos2d.h"
#include "Utils/DataHandler.h"

USING_NS_CC;

class Entity;
class ViewMode;

class ModelViewer : public cocos2d::Scene
{
public:
	CREATE_FUNC(ModelViewer);
	virtual bool init()override;
	virtual void update(float dt)override;

	void loadModel(ResourceDataList& animFileList);

	Entity*	getTarget();

	bool addViewTarget(const string& modelFilePath,const string& animFilePath,const string& texFilePath);
	bool checkTargetNameLegal(const string& modelName);
	void changeViewTarget(int step);
	void changeViewTarget(const std::string& targetName);
	bool deleteViewTarget(const string& targetName);

	void addNewAnim(const string& newAnimName);
	bool checkAnimNameLegal(const string& animName);
	void changeAnim(int step);
	void changeAnim(const std::string& animName);
	void modifyAnim(int start, int end);
	bool deleteAnim(const std::string& targetName,const std::string& animName);

	//Serialization
	bool saveAll();

	void toggleDebugDraw();

CC_CONSTRUCTOR_ACCESS:
    ModelViewer();
    virtual ~ModelViewer();

protected:
	void onEnter()override;
	void onExit()override;

	void initCamera();
	void initInput();

	void onTouchsMove(const std::vector<Touch*> &touchs, Event *event);
	void onMouseScroll(Event* event);
	void onMouseMove(Event* event);
	void onMouseDown(Event* event);
	void onMouseUp(Event* event);
	void onKeyPressed(EventKeyboard::KeyCode keycode, Event *event);
	void onKeyReleased(EventKeyboard::KeyCode keycode, Event *event);

	void addViewTarget(Entity*	newTarget);
	void updateUiAnimList();

protected:

	bool _debugDraw;
	int  _nextViewMode;

	//Ui
	RefPtr<class UiPage>	_uiPage;

	RefPtr<ViewMode> _vm;
	RefPtr<Camera>	_camera;
	RefPtr<Layer>	_modelLayer;

	//Entity manage
	int				_currViewTargetIdx;
	Vector<Entity*>	_viewTargetList;
};

#endif // __HELLOWORLD_SCENE_H__

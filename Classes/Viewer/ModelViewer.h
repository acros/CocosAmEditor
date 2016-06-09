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

	void initCamera();
	void loadModel(ResourceDataList& animFileList);


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

CC_CONSTRUCTOR_ACCESS:
    ModelViewer();
    virtual ~ModelViewer();

protected:
	void onEnter()override;
	void onExit()override;

	void initInput();
	void toggleDebugDraw();

    void trackball(Vec3 & axis, float & angle, float p1x, float p1y, float p2x, float p2y );
    float tb_project_to_sphere( float r, float x, float y );

    void updateCameraTransform();
    void resetCamera();

	void updateCameraSet();

	//Move this to ViewMode
	void onTouchsMovedThis(const std::vector<Touch*> &touchs, Event *event);
	void onMouseScrollThis(Event* event);
	void onMouseMovedThis(Event* event);
	void onKeyPressedThis(EventKeyboard::KeyCode keycode, Event *event);

private:
	bool _debugDraw;

	void addViewTarget(Entity*	newTarget);
	void updateUiAnimList();

	//Ui
	class UiPage*	_uiPage;

	class ViewMode*	 _vm;
	Camera*		_camera;
	Layer*		_modelLayer;

	//Camera set
	float		_orginDistance;
	Vec3		_orginCenter;
    float		_distance;
    float		_trackballSize;

	//Entity set
	Quaternion	_rotation;
	Vec3		_center;
	Vec2		_preMouseLocation;

	//Entity manage
	int				_currViewTargetIdx;
	Vector<Entity*>	_viewTargetList;
};

#endif // __HELLOWORLD_SCENE_H__
#pragma once

#include "cocos2d.h"
#include "Viewer/ModelViewer.h"

USING_NS_CC;

enum class ViewModeType{
	Unknown = 0,
	Normal,
	CameraControl,
	Count
};

class ViewMode : public Ref{
protected:
	ViewMode(ModelViewer& viewer, Camera& mainCam);
	virtual ~ViewMode()	{}

public:
	static ViewMode*	create(ModelViewer& mv,Camera& cam,ViewModeType type);

	ViewModeType getType()const	{ return _type; }

	virtual void update(float dt)	{}

	//Input
	virtual void onTouchsMove(const std::vector<Touch*> &touchs, Event *event)	{}
	virtual void onMouseScroll(Event* event)	{}
	virtual void onMouseMove(Event* event)	{}
	virtual void onMouseButtonEvent(Event* event, bool down) {}
	virtual void onKeyPressed(EventKeyboard::KeyCode keycode, Event *event);
	virtual void onKeyReleased(EventKeyboard::KeyCode keycode, Event *event);

	//Camera
	void updateCameraSet(Sprite3D* target);
	void resetCamera();

protected:
	ModelViewer&	_viewer;
	ViewModeType	_type;


	//Camera set
	Camera&		_mainCam;
	Vec3		_orginCenter;
	float		_orginDistance;
	float		_distance;
	Quaternion	_rotation;
	Vec3		_center;

	//Key flag
	bool	_altPressed;
	bool	_shiftPressed;
};
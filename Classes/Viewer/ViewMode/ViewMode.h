#pragma once

#include "cocos2d.h"
#include "Viewer/ModelViewer.h"

USING_NS_CC;

enum class ViewModeType{
	Unknown = 0,
	Normal,
	CameraControl
};

class ViewMode : public Ref{
protected:
	ViewMode(ModelViewer& viewer):_viewer(viewer)	{}
	virtual ~ViewMode()	{}

public:
	static ViewMode*	create(ModelViewer& mv,ViewModeType type);

	ViewModeType getType()const	{ return _type; }

	virtual void onTouchsMove(const std::vector<Touch*> &touchs, Event *event)=0;
	virtual void onMouseScroll(Event* event)=0;
	virtual void onMouseMove(Event* event)=0;
	virtual void onKeyPressed(EventKeyboard::KeyCode keycode, Event *event);

protected:
	ModelViewer&	_viewer;
	ViewModeType	_type;
};
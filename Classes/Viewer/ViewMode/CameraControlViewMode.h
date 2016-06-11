#pragma once

#include "ViewMode.h"

class CameraControlViewMode : public ViewMode{
CC_CONSTRUCTOR_ACCESS:
	CameraControlViewMode(ModelViewer& viewer);
	virtual ~CameraControlViewMode()	{}

public:
	static CameraControlViewMode*	create(ModelViewer& mv);

	virtual void onTouchsMove(const std::vector<Touch*> &touchs, Event *event)override;
	virtual void onMouseScroll(Event* event)override;
	virtual void onMouseMove(Event* event)override;
	virtual void onKeyPressed(EventKeyboard::KeyCode keycode, Event *event)override;
};
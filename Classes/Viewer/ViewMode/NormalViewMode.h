#pragma once

#include "ViewMode.h"

class NormalViewMode : public ViewMode{
CC_CONSTRUCTOR_ACCESS:
	NormalViewMode(ModelViewer& viewer);
	virtual ~NormalViewMode()	{}

public:
	static NormalViewMode*	create(ModelViewer& mv);

	virtual void onTouchsMove(const std::vector<Touch*> &touchs, Event *event)override;
	virtual void onMouseScroll(Event* event)override;
	virtual void onMouseMove(Event* event)override;
	virtual void onKeyPressed(EventKeyboard::KeyCode keycode, Event *event)override;

protected:

};
#pragma once

#include "ViewMode.h"

class CameraControlViewMode : public ViewMode{
CC_CONSTRUCTOR_ACCESS:
	CameraControlViewMode(ModelViewer& viewer, Camera& mainCam);
	virtual ~CameraControlViewMode()	{}

public:
	static CameraControlViewMode*	create(ModelViewer& mv, Camera& mainCam);

	virtual void update(float dt)override;

	virtual void onTouchsMove(const std::vector<Touch*> &touches, Event *event)override;
	virtual void onKeyPressed(EventKeyboard::KeyCode keycode, Event *event)override;
	virtual void onKeyReleased(EventKeyboard::KeyCode keycode, Event *event)override;

protected:

	int  _xMove;
	int  _yMove;
	int  _zMove;
	float _yawDegree;
	float _pitchDegree;

	float _moveSpeed;
	float _rotSensity;
};
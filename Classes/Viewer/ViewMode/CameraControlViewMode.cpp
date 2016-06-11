#include "CameraControlViewMode.h"

CameraControlViewMode* CameraControlViewMode::create(ModelViewer& mv)
{
	CameraControlViewMode *pRet = new(std::nothrow) CameraControlViewMode(mv);
	if (pRet)
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

CameraControlViewMode::CameraControlViewMode(ModelViewer& viewer) :ViewMode(viewer)
{
	_type = ViewModeType::CameraControl;
}

void CameraControlViewMode::onTouchsMove(const std::vector<Touch*> &touchs, Event *event)
{
	//Do nothing
}

void CameraControlViewMode::onMouseScroll(Event* event)
{
	//Do nothing
}

void CameraControlViewMode::onMouseMove(Event* event)
{
	//TODO: rotate camera 
}

void CameraControlViewMode::onKeyPressed(EventKeyboard::KeyCode keycode, Event *event)
{
	ViewMode::onKeyPressed(keycode, event);

	switch (keycode)
	{
	case EventKeyboard::KeyCode::KEY_SPACE:
		_viewer.resetCamera();
		break;
	default:
		break;
	}
}

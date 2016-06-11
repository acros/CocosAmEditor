#include "ViewMode.h"
#include "NormalViewMode.h"
#include "CameraControlViewMode.h"

ViewMode* ViewMode::create(ModelViewer& mv, ViewModeType type)
{
	if (type == ViewModeType::Normal)
	{
		return NormalViewMode::create(mv);
	}
	else if (type == ViewModeType::CameraControl)
	{
		return CameraControlViewMode::create(mv);
	}

	assert(false);
	return nullptr;
}

void ViewMode::onKeyPressed(EventKeyboard::KeyCode keycode, Event *event)
{
	switch (keycode)
	{
	case EventKeyboard::KeyCode::KEY_SPACE:
//		_viewer.resetCamera();
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		_viewer.changeViewTarget(1);
		break;
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		_viewer.changeViewTarget(-1);
		break;
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
		_viewer.changeAnim(1);
		break;
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		_viewer.changeAnim(-1);
		break;
	case EventKeyboard::KeyCode::KEY_F12:
		_viewer.toggleDebugDraw();
		break;
	case EventKeyboard::KeyCode::KEY_ALT:
		_altPressed = true;
		break;
	default:
		break;
	}
}

void ViewMode::onKeyReleased(EventKeyboard::KeyCode keycode, Event *event)
{
	switch (keycode)
	{
	case EventKeyboard::KeyCode::KEY_ALT:
		_altPressed = false;
		break;
	default:
		break;
	}
}

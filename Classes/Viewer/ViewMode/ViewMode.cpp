#include "ViewMode.h"
#include "NormalViewMode.h"
#include "CameraControlViewMode.h"

ViewMode::ViewMode(ModelViewer& viewer, Camera& mainCam) 
	: _viewer(viewer)
	, _mainCam(mainCam)
	, _altPressed(false)
	, _shiftPressed(false)
	, _distance(0.f)
	, _orginDistance(0.f)
{

}

ViewMode* ViewMode::create(ModelViewer& mv, Camera& cam, ViewModeType type)
{
	if (type == ViewModeType::Normal)
	{
		return NormalViewMode::create(mv,cam);
	}
	else if (type == ViewModeType::CameraControl)
	{
		return CameraControlViewMode::create(mv, cam);
	}

	assert(false);
	return nullptr;
}

void ViewMode::onKeyPressed(EventKeyboard::KeyCode keycode, Event *event)
{
	switch (keycode)
	{
	case EventKeyboard::KeyCode::KEY_SPACE:
		resetCamera();
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

void ViewMode::updateCameraSet(Sprite3D* target)
{
	const AABB& aabb = target->getAABB();
	OBB  obb(aabb);

	Vec3 corners[8];
	obb.getCorners(corners);

	if (abs(corners[3].x) == 99999.0f && abs(corners[3].y) == 99999.0f && abs(corners[3].z) == 99999.0f)
	{
		_orginCenter = Vec3(0.0f, 0.0f, 0.0f);
		_orginDistance = 100.0f;
	}
	else
	{
		float radius = (corners[0] - corners[5]).length();
		_orginCenter = (aabb._min + aabb._max) / 2;
		_orginDistance = radius;
	}

	resetCamera();
}

void ViewMode::resetCamera()
{
	_distance = _orginDistance;
	_center = _orginCenter;
	_rotation.set(0.0f, 0.0f, 0.0f, 1.0f);
	_mainCam.setPosition3D(_orginCenter + Vec3(0.0f, 0.0f, _orginDistance));
	_mainCam.lookAt(_orginCenter, Vec3(0.0f, 1.0f, 0.0f));
}

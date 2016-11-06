#include "CameraControlViewMode.h"

CameraControlViewMode* CameraControlViewMode::create(ModelViewer& mv, Camera& mainCam)
{
	CameraControlViewMode *pRet = new(std::nothrow) CameraControlViewMode(mv,mainCam);
	if (pRet)
	{
		pRet->autorelease();
		return pRet;
	}

	return nullptr;
}

CameraControlViewMode::CameraControlViewMode(ModelViewer& viewer, Camera& mainCam) 
	: ViewMode(viewer, mainCam)
	, _moveSpeed(1.f)
	, _rotSensity(0.2f)
	, _xMove(0)
	, _yMove(0)
	, _zMove(0)
	, _yawDegree(0.f)
	, _pitchDegree(0.f)
{
	_type = ViewModeType::CameraControl;
}

void CameraControlViewMode::update(float dt)
{
	//TODO:
// 	if (!isDirty)
// 		return;

	Mat4 camMat = _mainCam.getNodeToParentTransform();
	Vec3 up, fwd, right,pos;
	camMat.getForwardVector(&fwd);
	camMat.getUpVector(&up);
	camMat.getRightVector(&right);
	fwd.normalize();
	up.normalize();
	right.normalize();
	camMat.getTranslation(&pos);

	//Pos
	Vec3 deltaPos = fwd * _zMove * _moveSpeed + right * _xMove * _moveSpeed + up * _yMove * _moveSpeed;
	_mainCam.setPosition3D(deltaPos + pos);

	//////////////////////////////////////////////////////////////////////////
	//Rot
	bool updateRotAngle = false;
	Vec3 rot = _mainCam.getRotation3D();

	if (_yawDegree != 0.f)
	{
		updateRotAngle = true;
		float yawAngle = _yawDegree * _rotSensity;
		rot.x += yawAngle;

		_yawDegree = 0.f;
	}

	if (_pitchDegree != 0.f)
	{
		updateRotAngle = true;
		float pitchAngle = - _pitchDegree * _rotSensity;
		rot.y += pitchAngle;

		_pitchDegree = 0.f;
	}


	if (updateRotAngle)
	{
		_mainCam.setRotation3D(rot);
	}

}

void CameraControlViewMode::onTouchsMove(const std::vector<Touch*> &touches, Event *event)
{
	if (touches.size() == 1)
	{
		//Rotate camera
		Vec2 deltaRot = touches.at(0)->getDelta();
		_yawDegree += deltaRot.y;
		_pitchDegree += deltaRot.x;
	}
}

void CameraControlViewMode::onKeyPressed(EventKeyboard::KeyCode keycode, Event *event)
{
	ViewMode::onKeyPressed(keycode, event);

	switch (keycode)
	{
	case EventKeyboard::KeyCode::KEY_W:
		_zMove = 1;
		break;
	case EventKeyboard::KeyCode::KEY_S:
		_zMove = -1;
		break;
	case EventKeyboard::KeyCode::KEY_A:
		_xMove = -1;
		break;
	case EventKeyboard::KeyCode::KEY_D:
		_xMove = 1;
		break;
	case EventKeyboard::KeyCode::KEY_Q:
		_yMove = -1;
		break;
	case EventKeyboard::KeyCode::KEY_E:
		_yMove = 1;
		break;
	default:
		break;
	}

}

void CameraControlViewMode::onKeyReleased(EventKeyboard::KeyCode keycode, Event *event)
{
	ViewMode::onKeyReleased(keycode, event);

	switch (keycode)
	{
	case EventKeyboard::KeyCode::KEY_W:
	case EventKeyboard::KeyCode::KEY_S:
		_zMove = 0;
		break;
	case EventKeyboard::KeyCode::KEY_A:
	case EventKeyboard::KeyCode::KEY_D:
		_xMove = 0;
		break;
	case EventKeyboard::KeyCode::KEY_Q:
	case EventKeyboard::KeyCode::KEY_E:
		_yMove = 0;
		break;
	default:
		break;
	}
}

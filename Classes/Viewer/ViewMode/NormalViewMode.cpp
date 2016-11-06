#include "NormalViewMode.h"
#include "Entity/Entity.h"

NormalViewMode* NormalViewMode::create(ModelViewer& mv,Camera& cam)
{
	NormalViewMode *pRet = new(std::nothrow) NormalViewMode(mv,cam);
	if (pRet)
	{
		pRet->autorelease();
		return pRet;
	}

	return nullptr;
}

NormalViewMode::NormalViewMode(ModelViewer& viewer, Camera& cam)
	: ViewMode(viewer,cam)
	, _MoveViewInPlane(false)
{
	_type = ViewModeType::Normal;
}

NormalViewMode::~NormalViewMode()
{

}

void NormalViewMode::onTouchsMove(const std::vector<Touch*> &touchs, Event *event)
{
	if (!touchs.empty())
	{
		Vec2 ds = touchs.at(0)->getDelta();
		Sprite3D* spr = _viewer.getTarget()->getSprite3d();

		if (_altPressed)
		{
			//Move target around
			const Mat4& camMat = _mainCam.getNodeToWorldTransform();
			Vec3 rightDir,upDir;
			camMat.getRightVector(&rightDir);
			camMat.getUpVector(&upDir);
			rightDir.normalize();
			upDir.normalize();

			Vec3 pos = spr->getPosition3D();
			pos += (ds.x * 0.02f * rightDir);
			pos += (ds.y* 0.014 * upDir);

			spr->setPosition3D(pos);
		}
		else
		{
			//Target Rotate
			Mat4 prevMat = spr->getNodeToParentTransform();

			Vec3 localYaxis, localXaxis;
			prevMat.getUpVector(&localYaxis);

			prevMat.rotate(localYaxis, ds.x *  0.03f);		//Choose an acceptable rotate speed
			spr->setNodeToParentTransform(prevMat);
		}
	}
}

void NormalViewMode::onMouseScroll(Event* event)
{
 	EventMouse *em = dynamic_cast<EventMouse*>(event);
	if (em)
	{
		float dy = em->getScrollY();
		const Mat4 camMat = _mainCam.getNodeToWorldTransform();
		Vec3 fdDir;
		camMat.getForwardVector(&fdDir);
		fdDir.normalize();

		Sprite3D* spr = _viewer.getTarget()->getSprite3d();
		Vec3 pos = spr->getPosition3D();

		spr->setPosition3D(fdDir *dy + pos);
	}
}

void NormalViewMode::onMouseMove(Event* event)
{
	if (_MoveViewInPlane)
	{
		EventMouse* ev = static_cast<EventMouse*>(event);
		if (_prevMousePos.isZero())
		{
			_prevMousePos = ev->getLocation();
			return;
		}

		Vec2 deltaDis = ev->getLocation() - _prevMousePos ;
		_prevMousePos = ev->getLocation();

		//Calc the plane 
		Vec3  up, forward;
		const Mat4& camMat = _mainCam.getNodeToParentTransform();
		camMat.getUpVector(&up);
		up.normalize();

		Vec3 pos = _mainCam.getPosition3D();
		forward = _viewer.getTarget()->getSprite3d()->getPosition3D();
		forward -= pos;
		Vec3 right;
		Vec3::cross(up,forward, &right);
		right.normalize();

		float scaleValue = 0.2f;
		pos += scaleValue * (deltaDis.x * right + deltaDis.y * up);
		_mainCam.setPosition3D(pos);
	}
}
void NormalViewMode::onKeyPressed(EventKeyboard::KeyCode keycode, Event *event)
{
	ViewMode::onKeyPressed(keycode, event);

	/*
		switch (keycode)
		{
		default:
		break;
		}
	*/
}

void NormalViewMode::onMouseButtonEvent(Event* event, bool down)
{
	//Left button is handled in Touch event
	EventMouse* ev = static_cast<EventMouse*>(event);

	//Right button
	if (ev->getMouseButton() == 1)
	{
		_MoveViewInPlane = down;
		_prevMousePos.setZero();
	}
	
}

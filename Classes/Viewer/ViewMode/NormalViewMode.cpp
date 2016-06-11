#include "NormalViewMode.h"
#include "Entity/Entity.h"

NormalViewMode* NormalViewMode::create(ModelViewer& mv)
{
	NormalViewMode *pRet = new(std::nothrow) NormalViewMode(mv);
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

NormalViewMode::NormalViewMode(ModelViewer& viewer) 
	: ViewMode(viewer)
{
	_type = ViewModeType::Normal;
}

void NormalViewMode::onTouchsMove(const std::vector<Touch*> &touchs, Event *event)
{
	if (!touchs.empty())
	{
		Sprite3D* spr = _viewer.getTarget()->getSprite3d();
		Mat4 prevMat = spr->getNodeToParentTransform();

		Vec3 localYaxis,localXaxis;
		prevMat.getUpVector(&localYaxis);
		prevMat.getRightVector(&localXaxis);

		Vec2 ds = touchs.at(0)->getDelta();
		prevMat.rotate(localYaxis, ds.x *  0.03f);		//Choose an acceptable rotate speed
		spr->setNodeToParentTransform(prevMat);
	}
}

void NormalViewMode::onMouseScroll(Event* event)
{
 	EventMouse *em = dynamic_cast<EventMouse*>(event);
	if (em)
	{
		float dy = em->getScrollY();
		const Mat4 camMat = _viewer.getCamera()->getNodeToWorldTransform();
		Vec3 fdDir;
		camMat.getForwardVector(&fdDir);
		fdDir.normalize();

		Sprite3D* spr = _viewer.getTarget()->getSprite3d();
		Vec3 pos = spr->getPosition3D();

		static float scale = 1.f;
		spr->setPosition3D(fdDir *dy * scale + pos);
	}
}

void NormalViewMode::onMouseMove(Event* event)
{

}
void NormalViewMode::onKeyPressed(EventKeyboard::KeyCode keycode, Event *event)
{
	ViewMode::onKeyPressed(keycode, event);

	switch (keycode)
	{
	case EventKeyboard::KeyCode::KEY_SPACE:
		//TODO: reset model
//		_viewer.resetCamera();
		break;
	default:
		break;
	}
}
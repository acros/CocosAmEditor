#include "ViewTarget.h"
#include "UiHandler.h"
#include "IndexFileParser.h"

USING_NS_CC;

bool ViewTarget::init()
{
	return true;
}

ViewTarget::ViewTarget():
_orginDistance(0.0f)
, _MaxAnimFrame(0)
{

}

ViewTarget::~ViewTarget()
{

}

bool ViewTarget::load(ResData& fileIdx)
{
	if (fileIdx.texFile.empty())
		_Sprite3d = Sprite3D::create(fileIdx.modelFile);
	else
		_Sprite3d = Sprite3D::create(fileIdx.modelFile, fileIdx.texFile);

	if (_Sprite3d)
	{
		_name = fileIdx.name;
		_modelName = fileIdx.modelFile;
		auto animation = Animation3D::create(fileIdx.animFile);
		if (animation)
		{
			auto animate = Animate3D::create(animation);
			_AnimList.insert(IndexFileParser::s_DefaultAnim, animate);
			_MaxAnimFrame = animation->getDuration() * IndexFileParser::sFrameRate;
			UiHandler::getInstance()->setAnimName(IndexFileParser::s_DefaultAnim, 0, _MaxAnimFrame);
			_Sprite3d->runAction(RepeatForever::create(animate));

			parseAnimSection(fileIdx,animation);
		}

		AABB aabb = _Sprite3d->getAABB();
		_obbt = OBB(aabb);

		Vec3 corners[8];
		aabb.getCorners(corners);
		//temporary method, replace it
		if (abs(corners[3].x) == 99999.0f && abs(corners[3].y) == 99999.0f && abs(corners[3].z) == 99999.0f)
		{
			_orginCenter = Vec3(0.0f, 0.0f, 0.0f);
			_orginDistance = 100.0f;
		}
		else
		{
			float radius = (corners[0] - corners[5]).length();
			_orginCenter = aabb.getCenter();
			_orginDistance = radius;
		}

		_Sprite3d->setCameraMask((unsigned short)CameraFlag::USER1);
	}

	return true;
}

Sprite3D* ViewTarget::getNode() const
{
	return _Sprite3d;
}

void ViewTarget::switchAnim(int step)
{
	if (step == 0)
		return;

	if (_AnimList.size() <= 1)
		return;

	assert(step == 1 || step == -1);
	if (step > 0 )
	{
		_currAnim++;
		if (_currAnim == _AnimList.end()){
			_currAnim = _AnimList.begin();
		}		
	}
	else {
		if (_currAnim == _AnimList.begin()){
//			_currAnim = (--_AnimList.end());
		}
//		else
//			--_currAnim;
	}

	_Sprite3d->stopAllActions();
	_Sprite3d->runAction(RepeatForever::create(_currAnim->second));

	if (_currAnim == _AnimList.begin()){
		UiHandler::getInstance()->setAnimName(_currAnim->first, 0,_MaxAnimFrame);
	}
	else{
		auto animTarget = IndexFileParser::findAnim(_name,_currAnim->first);
		if (animTarget != nullptr)
			UiHandler::getInstance()->setAnimName(_currAnim->first, animTarget->start, animTarget->end);
	}
}

void ViewTarget::switchAnim(const std::string& animName)
{
	if (_AnimList.size() == 1){
		_currAnim = _AnimList.begin();
		return;
	}

	for (auto itr = _AnimList.begin(); itr != _AnimList.end();	++itr){
		if (itr->first == animName)	{
			_currAnim = itr;
			_Sprite3d->stopAllActions();
			_Sprite3d->runAction(RepeatForever::create(_currAnim->second));

			if (itr->first == IndexFileParser::s_DefaultAnim)
				UiHandler::getInstance()->setAnimName(_currAnim->first,0,_MaxAnimFrame);
			else{
				auto animTarget = IndexFileParser::findAnim(_name, animName);
				if (animTarget != nullptr)
					UiHandler::getInstance()->setAnimName(animName, animTarget->start, animTarget->end);
			}

			break;
		}
	}
}

void ViewTarget::parseAnimSection(const ResData& animFile, Animation3D* anim)
{
	for (auto it = animFile.animList.begin(); it != animFile.animList.end(); ++it){
		auto animate = Animate3D::createWithFrames(anim, it->start, it->end);
		_AnimList.insert(it->name, animate);
	}

	_currAnim = _AnimList.begin();
}

const std::string& ViewTarget::getCurrAnimName() const
{
	return _currAnim->first;
}

const std::string& ViewTarget::getTitle() const
{
	return _name;
}

const std::string& ViewTarget::getModelName() const
{
	return _modelName;
}

void ViewTarget::recreateCurrentAnim(int from, int to)
{
	auto viewData = IndexFileParser::findViewDate(_name);
	std::string animFilePath = viewData->animFile;
	if (animFilePath.empty())	{
		animFilePath = viewData->modelFile;
	}

	auto animation = Animation3D::create(animFilePath);
	auto animate = Animate3D::createWithFrames(animation,from,to);

	_Sprite3d->stopAllActions();
	_Sprite3d->runAction(RepeatForever::create(animate));

	auto animData = IndexFileParser::findAnim(_name, _currAnim->first);
	animData->start = from;
	animData->end = to;
	_AnimList.erase(_currAnim);
	_AnimList.insert(animData->name, animate);
	_currAnim = _AnimList.find(animData->name);
}

void ViewTarget::addNewAnimSection(const std::string& newAnimName)
{
	//For display
	auto _currAnimName = _currAnim->first;
	auto defaultAnim = _AnimList.find(IndexFileParser::s_DefaultAnim);
	auto newAnim = defaultAnim->second->clone();
	_AnimList.insert(newAnimName, newAnim);		
	_currAnim = _AnimList.find(_currAnimName);

	//For data
	IndexFileParser::findViewDate(_name)->animList.push_back(ResData::AnimFrames(newAnimName,0,_MaxAnimFrame));

	//For UI
	UiHandler::getInstance()->addAnimToViewList(newAnimName);
}

bool ViewTarget::removeCurrentAnim()
{
	if (_AnimList.size() <= 1)
		return false;

	auto currAnimName = _currAnim->first;
	_AnimList.erase(_currAnim);
	switchAnim(IndexFileParser::s_DefaultAnim);
	return true;
}

void ViewTarget::setDrawingBoundingBox(bool state)
{
	if (state)
	{
		if (_drawDebug == nullptr)
			_drawDebug = DrawNode3D::create();

		_drawDebug->setCameraMask((unsigned short)CameraFlag::USER1);
		_Sprite3d->addChild(_drawDebug);
	}
	else
	{
		if (_drawDebug != nullptr)
		{
			_Sprite3d->removeChild(_drawDebug);
			_drawDebug = nullptr;
		}
	}
}

void ViewTarget::update(float dt)
{
	if (_drawDebug)
	{
		_drawDebug->clear();

		Mat4 mat = _Sprite3d->getNodeToWorldTransform();
		mat.getRightVector(&_obbt._xAxis);
		_obbt._xAxis.normalize();

		mat.getUpVector(&_obbt._yAxis);
		_obbt._yAxis.normalize();

		mat.getForwardVector(&_obbt._zAxis);
		_obbt._zAxis.normalize();

		_obbt._center = _Sprite3d->getPosition3D();

		Vec3 corners[8] = {};
		_obbt.getCorners(corners);
		_drawDebug->drawCube(corners, Color4F(0, 1, 0, 1));
	}
}

#include "Entity.h"

USING_NS_CC;

Entity::Entity(): _TotalFrames(0)
{

}

Entity::~Entity()
{

}

// Entity* Entity::create(const string& modelPath, const string& animPath, const string& texPath)
// {
// 	Entity *pRet = new(std::nothrow) Entity();
// 	if (pRet && pRet->load(modelPath, animPath, texPath))
// 	{
// 		pRet->autorelease();
// 		return pRet;
// 	}
// 	else
// 	{
// 		delete pRet;
// 		pRet = nullptr;
// 		return nullptr;
// 	}
// }

Entity* Entity::create(const EntityData& data)
{
	Entity *pRet = new(std::nothrow) Entity();
	if (pRet && pRet->load(data))
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

// bool Entity::load(const string& modelPath, const string& animPath, const string& texPath)
// {
// 	auto newModelData = IndexFileParser::loadNewModel(modelPath,animPath,texPath);
// 	if (newModelData != nullptr){
// 		load(*newModelData);
// 		return true;
// 	}
// 
// 	return false;
// }

bool Entity::load(const EntityData& fileIdx)
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
			_AnimList.insert(DataHandler::s_DefaultAnim, animate);
			_TotalFrames = animation->getDuration() * DataHandler::sFrameRate;
			
			_Sprite3d->runAction(RepeatForever::create(animate));

			parseAnimSection(fileIdx,animation);
		}

		_obbt = OBB(_Sprite3d->getAABB());

		_Sprite3d->setCameraMask((unsigned short)CameraFlag::USER1);
	}

	return true;
}

void Entity::switchAnim(int step)
{
	if (step == 0)
		return;

	if (_AnimList.size() <= 1)
		return;

	assert(step == 1 || step == -1);
	auto itr = _currAnim;
	if (step > 0){
		if ( ++_currAnim == _AnimList.end()){
			_currAnim = itr;
		}		
	}
	else {
		if (_currAnim == _AnimList.begin()){
//			_currAnim = itr;
		}
		else{
			--_currAnim;
		}
	}

	_Sprite3d->stopAllActions();
	_Sprite3d->runAction(RepeatForever::create(_currAnim->second));
}

void Entity::switchAnim(const std::string& animName)
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

			//TODO: should set by viewer
			if (itr->first == DataHandler::s_DefaultAnim){
				//TODO:
			}

			break;
		}
	}
}

void Entity::parseAnimSection(const EntityData& animFile, Animation3D* anim)
{
	for (auto it = animFile.animList.begin(); it != animFile.animList.end(); ++it){
		auto animate = Animate3D::createWithFrames(anim, it->start, it->end);
		_AnimList.insert(it->name, animate);
	}

	_currAnim = _AnimList.begin();
}

void Entity::recreateCurrentAnim(int from, int to)
{
	auto viewData = DataHandler::findViewDate(_name);
	std::string animFilePath = viewData->animFile;
	if (animFilePath.empty())	{
		animFilePath = viewData->modelFile;
	}

	auto animation = Animation3D::create(animFilePath);
	auto animate = Animate3D::createWithFrames(animation,from,to);

	_Sprite3d->stopAllActions();
	_Sprite3d->runAction(RepeatForever::create(animate));

	auto animData = DataHandler::findAnim(_name, _currAnim->first);
	animData->start = from;
	animData->end = to;
	_AnimList.erase(_currAnim);
	_AnimList.insert(animData->name, animate);
	_currAnim = _AnimList.find(animData->name);
}

void Entity::addNewAnimSection(const std::string& newAnimName)
{
	//For display
	auto _currAnimName = _currAnim->first;
	auto defaultAnim = _AnimList.find(DataHandler::s_DefaultAnim);
	auto newAnim = defaultAnim->second->clone();
	_AnimList.insert(newAnimName, newAnim);		
	_currAnim = _AnimList.find(_currAnimName);

	//For data
	DataHandler::findViewDate(_name)->animList.push_back(EntityData::AnimFrames(newAnimName,0,_TotalFrames));
}

bool Entity::removeCurrentAnim()
{
	if (_AnimList.size() <= 1)
		return false;

	auto currAnimName = _currAnim->first;
	_AnimList.erase(_currAnim);
	switchAnim(DataHandler::s_DefaultAnim);
	return true;
}

void Entity::setDrawingBoundingBox(bool state)
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

void Entity::update(float dt)
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


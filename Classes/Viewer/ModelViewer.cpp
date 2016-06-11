#include "ModelViewer.h"
#include "Entity/Entity.h"
#include "Ui/UiPage.h"
#include "ViewMode/ViewMode.h"

USING_NS_CC;

bool ModelViewer::init()
{
    if ( !Scene::init() )
        return false;

	auto animFileData = DataHandler::deserializeFromFile("config.json");

	if (animFileData != nullptr)
	{
		//ui layer
		_uiPage = UiPage::create(*this);
		addChild(_uiPage);

		//3d model layer
		_modelLayer = Layer::create();
		addChild(_modelLayer);
		initCamera();

		//TODO: Should not init model data by viewer
		loadModel(*animFileData);
	}

	_vm = ViewMode::create(*this,ViewModeType::Normal);

	initInput();

    return true;
}

void ModelViewer::initInput()
{
	auto listenertouch = EventListenerTouchAllAtOnce::create();
	listenertouch->onTouchesMoved = CC_CALLBACK_2(ModelViewer::onTouchsMove, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenertouch, this);

	auto listenermouse = EventListenerMouse::create();
	listenermouse->onMouseScroll = CC_CALLBACK_1(ModelViewer::onMouseScroll, this);
	listenermouse->onMouseMove = CC_CALLBACK_1(ModelViewer::onMouseMove, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenermouse, this);

	auto listenerkeyboard = EventListenerKeyboard::create();
	listenerkeyboard->onKeyPressed = CC_CALLBACK_2(ModelViewer::onKeyPressed, this);
	listenerkeyboard->onKeyReleased = CC_CALLBACK_2(ModelViewer::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenerkeyboard, this);
}

void ModelViewer::loadModel(ResourceDataList& animFileList)
{
	FileUtils::getInstance()->addSearchPath("data");

	for (auto spr = animFileList.begin(); spr != animFileList.end(); ++spr)
	{
		//TODO: construct filename with path, no need to add to search path.
		FileUtils::getInstance()->addSearchPath(spr->name);
		FileUtils::getInstance()->addSearchPath("data/" + spr->name);

		auto target = Entity::create(*spr);
		if (target != nullptr)
		{
			addViewTarget(target);
		}
	}

	_currViewTargetIdx = 0;
	changeViewTarget(0);
}

ModelViewer::ModelViewer()
    : _camera(nullptr)
    , _modelLayer(nullptr)
    , _distance(0.0f)
    , _trackballSize(1.0f)
	, _debugDraw(false)
	, _currViewTargetIdx(0)
	, _vm(nullptr)
{

}

ModelViewer::~ModelViewer()
{
}

void ModelViewer::onTouchsMove( const std::vector<Touch*> &touchs, Event *event )
{
	_vm->onTouchsMove(touchs, event);
}

void ModelViewer::onMouseScroll( Event* event )
{
	_vm->onMouseScroll(event);
}

void ModelViewer::onMouseMove( Event* event )
{
	_vm->onMouseMove(event);
}

void ModelViewer::onKeyPressed(EventKeyboard::KeyCode keycode, Event *event)
{
	_vm->onKeyPressed(keycode, event);
}

void ModelViewer::onKeyReleased(EventKeyboard::KeyCode keycode, Event *event)
{
	_vm->onKeyReleased(keycode, event);

}

float ModelViewer::tb_project_to_sphere( float r, float x, float y )
{
    float d, t, z;
    d = sqrt(x*x + y*y);
    if (d < r * 0.70710678118654752440)
    {
        z = sqrt(r*r - d*d);
    }                         
    else
    {
        t = r / 1.41421356237309504880;
        z = t*t / d;
    }
    return z;
}

void ModelViewer::trackball( cocos2d::Vec3 & axis, float & angle, float p1x, float p1y, float p2x, float p2y )
{
    Mat4 rotation_matrix;
    Mat4::createRotation(_rotation, &rotation_matrix);

    Vec3 uv = rotation_matrix * Vec3(0.0f,1.0f,0.0f);
    Vec3 sv = rotation_matrix * Vec3(1.0f,0.0f,0.0f);
    Vec3 lv = rotation_matrix * Vec3(0.0f,0.0f,-1.0f);

    Vec3 p1 = sv * p1x + uv * p1y - lv * tb_project_to_sphere(_trackballSize, p1x, p1y);
    Vec3 p2 = sv * p2x + uv * p2y - lv * tb_project_to_sphere(_trackballSize, p2x, p2y);

    Vec3::cross(p2, p1, &axis);
    axis.normalize();

    float t = (p2 - p1).length() / (2.0 * _trackballSize);

    if (t > 1.0) t = 1.0;
    if (t < -1.0) t = -1.0;
    angle = asin(t);
}

void ModelViewer::updateCameraTransform()
{
    Mat4 trans, rot, center;
    Mat4::createTranslation(Vec3(0.0f, 0.0f, _distance), &trans);
    Mat4::createRotation(_rotation, &rot);
    Mat4::createTranslation(_center, &center);
    Mat4 result = center * rot * trans;
    _camera->setNodeToParentTransform(result);
}

void ModelViewer::resetCamera()
{
    _distance = _orginDistance;
    _center = _orginCenter;
    _rotation.set(0.0f, 0.0f, 0.0f, 1.0f);
    _camera->setPosition3D(_orginCenter + Vec3(0.0f, 0.0f,  _orginDistance));
    _camera->lookAt(_orginCenter, Vec3(0.0f, 1.0f, 0.0f));
}

void ModelViewer::initCamera()
{
	const Size& visibleSize = Director::getInstance()->getVisibleSize();
	_camera = Camera::createPerspective(60.0f, (GLfloat)visibleSize.width / visibleSize.height, 1.0f, _orginDistance * 5.0f);
	_camera->setCameraFlag(CameraFlag::USER1);
	_camera->setPosition3D(Vec3(0.0f, 0.0f, 10.0f));
	_camera->lookAt(Vec3::ZERO, Vec3::UNIT_Y);
	_camera->setCameraMask((unsigned short)CameraFlag::USER1);
	addChild(_camera);
}

void ModelViewer::changeViewTarget(int stepLength)
{
	if (_viewTargetList.size() <= 1)
		return;

	_uiPage->clearAnimViewList();

	_viewTargetList.at(_currViewTargetIdx)->getSprite3d()->removeFromParentAndCleanup(false);

	stepLength %= _viewTargetList.size();
	_currViewTargetIdx += stepLength;
	if (_currViewTargetIdx >= _viewTargetList.size())
	{
		_currViewTargetIdx %= _viewTargetList.size();
	}
	else if (_currViewTargetIdx < 0){
		_currViewTargetIdx += _viewTargetList.size();
	}

	addChild(_viewTargetList.at(_currViewTargetIdx)->getSprite3d());

	updateCameraSet();
	updateUiAnimList();
}

void ModelViewer::changeViewTarget(const std::string& targetName)
{
	if (_viewTargetList.size() <= 1)
		return;

	for (int i = 0; i < _viewTargetList.size(); ++i){
		if (_viewTargetList.at(i)->getFriendlyName() == targetName){
			_viewTargetList.at(_currViewTargetIdx)->getSprite3d()->removeFromParentAndCleanup(false);
		
			_uiPage->clearAnimViewList();
			_currViewTargetIdx = i;
			addChild(_viewTargetList.at(_currViewTargetIdx)->getSprite3d());

			updateCameraSet();
			updateUiAnimList();
			break;
		}
	}
}

void ModelViewer::updateCameraSet()
{
//	_orginDistance = _viewTargetList.at(_currViewTargetIdx)->getCamDistance();
//	_orginCenter.set(_viewTargetList.at(_currViewTargetIdx)->getCamCenter());

	auto spr = _viewTargetList.at(_currViewTargetIdx)->getSprite3d();
	AABB aabb = spr->getAABB();
	OBB  obb = OBB(aabb);

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
		_orginCenter = aabb.getCenter();
		_orginDistance = radius;
	}


	resetCamera();
}

void ModelViewer::changeAnim(int step)
{
	Entity* ent = _viewTargetList.at(_currViewTargetIdx);
	ent->switchAnim(step);

	//TODO: process of default animation
	auto animTarget = DataHandler::findAnim(ent->getFriendlyName(), ent->getCurrAnimName());
	if (animTarget != nullptr)
		_uiPage->setAnimData(ent->getCurrAnimName(), animTarget->start, animTarget->end);
}

void ModelViewer::changeAnim(const std::string& animName)
{
	_viewTargetList.at(_currViewTargetIdx)->switchAnim(animName);

	//TODO: default animation

	auto animTarget = DataHandler::findAnim(_name, animName);
	if (animTarget != nullptr)
		_uiPage->setAnimData(animName, animTarget->start, animTarget->end);
}

void ModelViewer::modifyAnim(int start, int end)
{
	_viewTargetList.at(_currViewTargetIdx)->recreateCurrentAnim(start, end);
}

bool ModelViewer::deleteAnim(const std::string& targetName,const std::string& animName)
{
	//TODO: Delete item from data source
	auto viewData = DataHandler::findViewDate(targetName);
	for (auto itr = viewData->animList.begin(); itr != viewData->animList.end(); ++itr){
		if (itr->name == animName){
			viewData->animList.erase(itr);
			break;
		}
	}

	return  _viewTargetList.at(_currViewTargetIdx)->removeCurrentAnim();
}

bool ModelViewer::addViewTarget(const string& modelFilePath, const string& animFilePath, const string& texFilePath)
{
	assert(false);
// 		auto newTarget = Entity::create(uiInfo->_info, uiInfo->_info, uiInfo->_info2);
// 		if (newTarget != nullptr)
// 			addViewTarget(newTarget);
	return false;
}


bool ModelViewer::deleteViewTarget(const string& targetName)
{
	if (_viewTargetList.size() <= 1){
		return false;
//		_uiPage->showUserMsg("The only model left here, please don't kill me. >_<");
	}
	else{

		//Delete the data source
		for (auto itr = DataHandler::s_AnimFileData.begin(); itr != DataHandler::s_AnimFileData.end(); ++itr){
			if (itr->name == targetName){
				DataHandler::s_AnimFileData.erase(itr);
				break;
			}
		}

		for (auto itr = _viewTargetList.begin(); itr != _viewTargetList.end(); ++itr){
			if ((*itr)->getFriendlyName() == targetName){
				(*itr)->getSprite3d()->removeFromParent();
				_viewTargetList.erase(itr);
				break;
			}
		}

		_currViewTargetIdx = 0;
		addChild(_viewTargetList.at(_currViewTargetIdx)->getSprite3d());
		updateCameraSet();
		updateUiAnimList();
	}

	return true;
}

void ModelViewer::addViewTarget(Entity* newTarget)
{
	_viewTargetList.pushBack(newTarget);
	_uiPage->addModelToViewList(newTarget->getFriendlyName());
}

void ModelViewer::addNewAnim(const string& newAnimName)
{
	_viewTargetList.at(_currViewTargetIdx)->addNewAnimSection(newAnimName);
}

void ModelViewer::updateUiAnimList()
{
	_uiPage->setFriendlyName(_viewTargetList.at(_currViewTargetIdx)->getFriendlyName());
	_uiPage->setModelName(_viewTargetList.at(_currViewTargetIdx)->getModelName());

	auto animMap = _viewTargetList.at(_currViewTargetIdx)->getAnimMap();
	for (auto itr = animMap.begin(); itr != animMap.end();	++itr){
		_uiPage->addAnimToViewList(itr->first);
	}

	changeAnim(DataHandler::s_DefaultAnim);
	_uiPage->setAnimData(DataHandler::s_DefaultAnim, 0, _viewTargetList.at(_currViewTargetIdx)->getFramesCount());
}

void ModelViewer::update(float dt)
{
	if (_currViewTargetIdx >=0 && _currViewTargetIdx < _viewTargetList.size())
	{
		_viewTargetList.at(_currViewTargetIdx)->update(dt);
	}
}

void ModelViewer::onEnter()
{
	Scene::onEnter();

	scheduleUpdate();
}

void ModelViewer::onExit()
{
	Scene::onExit();

	unscheduleUpdate();
}

void ModelViewer::toggleDebugDraw()
{
	if (_currViewTargetIdx >= 0 && _currViewTargetIdx < _viewTargetList.size())
	{
		_debugDraw = !_debugDraw;
		_viewTargetList.at(_currViewTargetIdx)->setDrawingBoundingBox(_debugDraw);
	}
}

bool ModelViewer::saveAll()
{
	return DataHandler::serializeToFile();
}

bool ModelViewer::checkTargetNameLegal(const string& modelName)
{
	bool result = true;
	for (auto& items : DataHandler::s_AnimFileData){
		string newName = modelName;
		string oldName(items.name);
		std::transform(newName.begin(), newName.end(), newName.begin(), tolower);
		std::transform(oldName.begin(), oldName.end(), oldName.begin(), tolower);

		if (oldName == newName){
			result = false;
			break;
		}

	}

	return result;
}

bool ModelViewer::checkAnimNameLegal(const string& animName)
{
	bool result = true;
	for (auto& items : DataHandler::s_AnimFileData){
		if (items.name == animName)	{
			for (auto& anim : items.animList){
				string newName = animName;
				string oldName(anim.name);
				std::transform(newName.begin(), newName.end(), newName.begin(), tolower);
				std::transform(oldName.begin(), oldName.end(), oldName.begin(), tolower);
				if (oldName == newName){
					result = false;
					break;
				}
			}
		}
	}

	return result;
}

Entity* ModelViewer::getTarget()
{
	if (_currViewTargetIdx >= 0 && _currViewTargetIdx < _viewTargetList.size())
	{
		return _viewTargetList.at(_currViewTargetIdx);
	}
	
	return nullptr;
}






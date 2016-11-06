#include "ModelViewer.h"
#include "Entity/Entity.h"
#include "Ui/UiPage.h"
#include "ViewMode/ViewMode.h"

USING_NS_CC;

bool ModelViewer::init()
{
    if ( !Scene::init() )
        return false;

	initCamera();
	_vm = ViewMode::create(*this, *(_camera.get()), ViewModeType::Normal);

	auto animFileData = DataHandler::deserializeFromFile("config.json");
	if (animFileData != nullptr)
	{
		//ui layer
		_uiPage = UiPage::create(*this);
		addChild(_uiPage);

		//3d model layer
		_modelLayer = Layer::create();
		addChild(_modelLayer);

		//TODO: Should not init model data by viewer
		loadModel(*animFileData);
	}

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
	listenermouse->onMouseDown = CC_CALLBACK_1(ModelViewer::onMouseDown, this);
	listenermouse->onMouseUp = CC_CALLBACK_1(ModelViewer::onMouseUp, this);
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
	, _debugDraw(false)
	, _currViewTargetIdx(0)
	, _vm(nullptr)
	, _nextViewMode(0)
{

}

ModelViewer::~ModelViewer()
{
}

void ModelViewer::onTouchsMove( const std::vector<Touch*> &touchs, Event *event )
{
	if (touchs.size() == 1)
	{
		_vm->onTouchsMove(touchs, event);
	}
}

void ModelViewer::onMouseScroll( Event* event )
{
	_vm->onMouseScroll(event);
}

void ModelViewer::onMouseDown(Event* event)
{
	_vm->onMouseButtonEvent(event, true);
}

void ModelViewer::onMouseUp(Event* event)
{
	_vm->onMouseButtonEvent(event, false);
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

	if (keycode == EventKeyboard::KeyCode::KEY_F11)
	{
		_nextViewMode = ((int)_vm->getType() + 1 ) % (int)ViewModeType::Count;
		if (_nextViewMode == 0)
			_nextViewMode += 1;
	}
}

void ModelViewer::initCamera()
{
	const Size& visibleSize = Director::getInstance()->getVisibleSize();
	_camera = Camera::createPerspective(60.0f, (GLfloat)visibleSize.width / visibleSize.height, 1.0f, 5000.f);
	
	addChild(_camera);

	_camera->setCameraFlag(CameraFlag::USER1);
	_camera->setCameraMask((unsigned short)CameraFlag::USER1);
	
	_camera->setPosition3D(Vec3(0.0f, 0.0f, 10.0f));
	_camera->lookAt(Vec3::ZERO, Vec3::UNIT_Y);
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

	auto spr = _viewTargetList.at(_currViewTargetIdx)->getSprite3d();
	addChild(spr);
	_vm->updateCameraSet(spr);

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
			
			auto spr = _viewTargetList.at(_currViewTargetIdx)->getSprite3d();
			addChild(spr);
			_vm->updateCameraSet(spr);

			updateUiAnimList();
			break;
		}
	}
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
		auto spr = _viewTargetList.at(_currViewTargetIdx)->getSprite3d();
		addChild(spr);
		_vm->updateCameraSet(spr);
		
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

	_vm->update(dt);

	if (_nextViewMode > 0)
	{
		if ((int)_vm->getType() !=  _nextViewMode)
		{
			_vm = ViewMode::create(*this, *_camera, ViewModeType(_nextViewMode));
		}

		_nextViewMode = 0;
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




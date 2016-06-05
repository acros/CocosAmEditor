#include "ModelViewer.h"
#include "ViewTarget.h"
#include "UiHandler.h"

USING_NS_CC;

Scene* ModelViewer::createScene(const std::string& filePath)
{
	auto animFileData = IndexFileParser::parseIndexFile(filePath);

	if (animFileData != nullptr)
	{
		auto scene = Scene::create();

		//Add ui lay
		auto hud = UiHandler::create();
		scene->addChild(hud);

		//Add model layer
		auto layer = ModelViewer::create();
 		layer->initCamera();
		layer->loadModel(*animFileData);
		scene->addChild(layer);

		return scene;
	}

	return nullptr;
}

// on "init" you need to initialize your instance
bool ModelViewer::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    auto listenertouch = EventListenerTouchAllAtOnce::create();
    listenertouch->onTouchesMoved = CC_CALLBACK_2(ModelViewer::onTouchsMovedThis, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listenertouch, this);

    auto listenermouse= EventListenerMouse::create();
    listenermouse->onMouseScroll = CC_CALLBACK_1(ModelViewer::onMouseScrollThis, this);
    listenermouse->onMouseMove = CC_CALLBACK_1(ModelViewer::onMouseMovedThis, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listenermouse, this);

    auto listenerkeyboard= EventListenerKeyboard::create();
    listenerkeyboard->onKeyPressed = CC_CALLBACK_2(ModelViewer::onKeyPressedThis, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listenerkeyboard, this);

	auto listenerOfUiEvent = EventListenerCustom::create(UiCustomEventData::sUiCustomEventName, CC_CALLBACK_1(ModelViewer::onUiCustomEvent, this));
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenerOfUiEvent, this);

    _layer = Layer::create();
    _layer->retain();
    addChild(_layer);

    return true;
}

void ModelViewer::loadModel(ResourceDataList& animFileList)
{
	FileUtils::getInstance()->addSearchPath("data");

	for (auto spr = animFileList.begin(); spr != animFileList.end(); ++spr)
	{
		FileUtils::getInstance()->addSearchPath(spr->name);
		FileUtils::getInstance()->addSearchPath("data/" + spr->name);

		auto target = ViewTarget::create();
		target->load(*spr);

		addViewTarget(target);
	}

	_currViewTargetIdx = 0;
	changeViewTarget(0);
}

ModelViewer::ModelViewer()
    : _camera(nullptr)
    , _layer(nullptr)
    , _distance(0.0f)
    , _trackballSize(1.0f)
	, _debugDraw(false)
{

}

ModelViewer::~ModelViewer()
{
    if (_layer)
        _layer->release();

    if (_camera)
        _camera->release();
}

void ModelViewer::onTouchsMovedThis( const std::vector<Touch*> &touchs, Event *event )
{
    if (!touchs.empty())
    {
        Size visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 prelocation = touchs[0]->getPreviousLocationInView();
        Vec2 location = touchs[0]->getLocationInView();
        location.x = 2.0 * (location.x) / (visibleSize.width) - 1.0f;
        location.y = 2.0 * (visibleSize.height - location.y) / (visibleSize.height) - 1.0f;
        prelocation.x = 2.0 * (prelocation.x) / (visibleSize.width) - 1.0f;
        prelocation.y = 2.0 * (visibleSize.height - prelocation.y) / (visibleSize.height) - 1.0f;

        Vec3 axes;
        float angle;
        trackball(axes, angle, prelocation.x, prelocation.y, location.x, location.y);
        Quaternion quat(axes, angle);
        _rotation = quat * _rotation;

        updateCameraTransform();
    }
}

void ModelViewer::onMouseScrollThis( Event* event )
{
    EventMouse *em = dynamic_cast<EventMouse *>(event);
    if (em)
    {
        _distance += em->getScrollY() * _orginDistance * 0.01f;

        updateCameraTransform();
    }
}

void ModelViewer::onMouseMovedThis( Event* event )
{
    EventMouse *em = dynamic_cast<EventMouse *>(event);
    if (em)
    {
        
        if (em->getMouseButton() == GLFW_MOUSE_BUTTON_RIGHT)
        {
            Size visibleSize = Director::getInstance()->getVisibleSize();
            Vec2 delta = (em->getLocation() - _preMouseLocation);
            Mat4 rot;
            Mat4::createRotation(_rotation, &rot);
            _center += rot * -Vec3(delta.x / visibleSize.width, (-delta.y) / visibleSize.height, 0.0f) * _orginDistance * 0.1f;
            updateCameraTransform();
        }

        _preMouseLocation = em->getLocation();
        
    }
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

void ModelViewer::onKeyPressedThis( EventKeyboard::KeyCode keycode, Event *event )
{
    switch (keycode)
    {
    case EventKeyboard::KeyCode::KEY_SPACE:
            resetCamera();
        break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
			changeViewTarget(1);
		break;
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
			changeViewTarget(-1);
		break;
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
			changeAnim(-1);
		break;
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
			changeAnim(1);
		break;
	case EventKeyboard::KeyCode::KEY_F12:
		toggleDebugDraw();
		break;
    default:
        break;
    }
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
    Size visibleSize = Director::getInstance()->getVisibleSize();
    _camera = Camera::createPerspective(60.0f, (GLfloat)visibleSize.width/visibleSize.height, 1.0f, _orginDistance * 5.0f);
    _camera->setCameraFlag(CameraFlag::USER1);
    _camera->setPosition3D(Vec3(0.0f, 0.0f, 10.0f));
    _camera->lookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
    _camera->retain();
    _camera->setCameraMask((unsigned short)CameraFlag::USER1);
    _layer->addChild(_camera);
}

void ModelViewer::changeViewTarget(int stepLength)
{
	if (_viewTargetList.size() <= 1)
		return;

	UiHandler::getInstance()->clearAnimViewList();

	_viewTargetList.at(_currViewTargetIdx)->getNode()->removeFromParentAndCleanup(false);

	stepLength %= _viewTargetList.size();
	_currViewTargetIdx += stepLength;
	if (_currViewTargetIdx >= _viewTargetList.size())
	{
		_currViewTargetIdx %= _viewTargetList.size();
	}
	else if (_currViewTargetIdx < 0){
		_currViewTargetIdx += _viewTargetList.size();
	}

	addChild(_viewTargetList.at(_currViewTargetIdx)->getNode());

	updateCameraSet();
	updateUiAnimList();
}

void ModelViewer::changeViewTarget(const std::string& targetName)
{
	if (_viewTargetList.size() <= 1)
		return;

	for (int i = 0; i < _viewTargetList.size(); ++i){
		if (_viewTargetList.at(i)->getTitle() == targetName){
			_viewTargetList.at(_currViewTargetIdx)->getNode()->removeFromParentAndCleanup(false);
		
			UiHandler::getInstance()->clearAnimViewList();
			_currViewTargetIdx = i;
			addChild(_viewTargetList.at(_currViewTargetIdx)->getNode());

			updateCameraSet();
			updateUiAnimList();
			break;
		}
	}
}

void ModelViewer::updateCameraSet()
{
	_orginDistance = _viewTargetList.at(_currViewTargetIdx)->getCamDistance();
	_orginCenter.set(_viewTargetList.at(_currViewTargetIdx)->getCamCenter());

	resetCamera();
}

void ModelViewer::changeAnim(int step)
{
	_viewTargetList.at(_currViewTargetIdx)->switchAnim(step);
}

void ModelViewer::changeAnim(const std::string& animName)
{
	_viewTargetList.at(_currViewTargetIdx)->switchAnim(animName);
}

void ModelViewer::onUiCustomEvent(cocos2d::EventCustom* event)
{
	auto uiInfo = static_cast<UiCustomEventData*>(event->getUserData());

	switch (uiInfo->_type)
	{
	case UiCustomEventType::UCE_SELECT_MODEL:
		if (uiInfo->_int1 != 0)
			changeViewTarget(uiInfo->_info);
		break;
	case UiCustomEventType::UCE_SELECT_ANIM:
			changeAnim(uiInfo->_info);
		break;
	case UiCustomEventType::UCE_MODIFY_CURRANT_ANIM:
			_viewTargetList.at(_currViewTargetIdx)->recreateCurrentAnim(uiInfo->_int1, uiInfo->_int2);
		break;
	case  UiCustomEventType::UCE_ADD_ANIM:
			_viewTargetList.at(_currViewTargetIdx)->addNewAnimSection(uiInfo->_info);
		break;
	case UiCustomEventType::UCE_ADD_MODEL:
	{
		auto newModelData = IndexFileParser::loadNewModel(uiInfo->_info,uiInfo->_info2);
		if (newModelData != nullptr){
			ViewTarget*	newTarget = ViewTarget::create();
			newTarget->load(*newModelData);
			addViewTarget(newTarget);
		}else
			UiHandler::getInstance()->showUserMsg("File not found, please ensure the files are in data folder!", Color3B::RED);
	}
		break;
	case UiCustomEventType::UCE_DELETE_ANIM:
	{
		if(_viewTargetList.at(_currViewTargetIdx)->removeCurrentAnim())
			UiHandler::getInstance()->showUserMsg("Animation deleted.", Color3B::GREEN);
		else
			UiHandler::getInstance()->showUserMsg("Can't delete default animation.", Color3B::RED);
	}
		break;

	case UiCustomEventType::UCE_DELETE_MODEL:
	{
		if (_viewTargetList.size() <= 1 ){
			UiHandler::getInstance()->showUserMsg("The only model left here, please don't kill me. >_<");
		}
		else{
			for (auto itr = _viewTargetList.begin(); itr != _viewTargetList.end(); ++itr){
				if ((*itr)->getTitle() == uiInfo->_info){
					(*itr)->getNode()->removeFromParent();
					_viewTargetList.erase(itr);
					break;
				}
			}

			UiHandler::getInstance()->clearAnimViewList();
			_currViewTargetIdx = 0;
			addChild(_viewTargetList.at(_currViewTargetIdx)->getNode());
			updateCameraSet();
			updateUiAnimList();

			UiHandler::getInstance()->showUserMsg("Model deleted.", Color3B::GREEN);
		}
	}
		break;
	default:
		break;
	}

}

void ModelViewer::addViewTarget(ViewTarget* newTarget)
{
	_viewTargetList.pushBack(newTarget);
	UiHandler::getInstance()->addModelToViewList(newTarget->getTitle());
}

void ModelViewer::updateUiAnimList()
{
	UiHandler::getInstance()->setTitle(_viewTargetList.at(_currViewTargetIdx)->getTitle());
	UiHandler::getInstance()->setModelName(_viewTargetList.at(_currViewTargetIdx)->getModelName());

	auto animMap = _viewTargetList.at(_currViewTargetIdx)->getAnimMap();
	for (auto itr = animMap.begin(); itr != animMap.end();	++itr){
		UiHandler::getInstance()->addAnimToViewList(itr->first);
	}

	changeAnim(IndexFileParser::s_DefaultAnim);
	UiHandler::getInstance()->setAnimName(IndexFileParser::s_DefaultAnim, 0, _viewTargetList.at(_currViewTargetIdx)->getMaxFrame());
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
	Layer::onEnter();

	scheduleUpdate();
}

void ModelViewer::onExit()
{
	Layer::onExit();

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


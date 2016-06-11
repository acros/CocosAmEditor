#include "AppDelegate.h"
#include "Viewer/ModelViewer.h"

USING_NS_CC;

AppDelegate::AppDelegate()
    : _modelFilePath("config.json")
{

}

//Hard-code
AppDelegate::AppDelegate(const std::string& fileName)
: _modelFilePath("config.json")
{
    
}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {

	auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
//		Rect rec(0, 0, 480, 320);
//		glview = GLViewImpl::createWithRect("Model Viewer",rec);
		glview = GLViewImpl::create("Cocos2d-x Model Viewer");
        director->setOpenGLView(glview);
    }

    director->setDisplayStats(true);
    director->setAnimationInterval(float(1.0 / 60));

    auto scene = ModelViewer::create();
	if (scene != nullptr)
		director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}

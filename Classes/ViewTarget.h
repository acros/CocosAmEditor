#ifndef __VIEW_TARGET_H__
#define __VIEW_TARGET_H__

#include "cocos2d.h"
#include "IndexFileParser.h"
#include "NodeDraw/DrawNode3D.h"

typedef cocos2d::Map<std::string, cocos2d::Animate3D*>::iterator	AnimMapIter;
typedef cocos2d::Map<std::string, cocos2d::Animate3D*>		AnimMap;

USING_NS_CC;

class ViewTarget : public cocos2d::Ref{
CC_CONSTRUCTOR_ACCESS:
	ViewTarget();
	virtual ~ViewTarget();

	virtual bool init();

public:
	CREATE_FUNC(ViewTarget);

	bool	load(ResData&	animFile);

	Sprite3D*	getNode()const;

	float	getCamDistance()const	{ return _orginDistance; }
	const	cocos2d::Vec3&	getCamCenter()const	{ return _orginCenter; }

	void	switchAnim(int step);
	void	switchAnim(const std::string& animName);

	const std::string& getTitle()const;
	const std::string& getModelName()const;
	const std::string& getCurrAnimName()const;

	const AnimMap&	getAnimMap()const	{ return _AnimList; }

	int	getMaxFrame()const { return _MaxAnimFrame; }
	void recreateCurrentAnim(int from, int to);

	void addNewAnimSection(const std::string& newAnimName);
	bool removeCurrentAnim();

	void setDrawingBoundingBox(bool state);

	void update(float dt);

protected:
	void	parseAnimSection(const ResData&	animFile, cocos2d::Animation3D* anim);

	float			_orginDistance;
	Vec3			_orginCenter;

	RefPtr<Sprite3D>	_Sprite3d;
	OBB					_obbt;
	RefPtr<DrawNode3D>  _drawDebug;

	std::string	_name;
	std::string _modelName;

	AnimMap	_AnimList;
	AnimMapIter _currAnim;

	int		_MaxAnimFrame;
};

#endif
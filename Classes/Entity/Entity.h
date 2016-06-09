#ifndef __VIEW_TARGET_H__
#define __VIEW_TARGET_H__

#include "cocos2d.h"
#include "Utils/DataHandler.h"
#include "Utils/DrawNode3D.h"

USING_NS_CC;


class Entity : public cocos2d::Ref{

	typedef Map<std::string, Animate3D*>::iterator	AnimMapIter;
	typedef Map<std::string, Animate3D*>				AnimMap;

CC_CONSTRUCTOR_ACCESS:
	Entity();
	virtual ~Entity();

public:
//	static Entity* create(const string& modelPath,const string& animPath,const string& texPath);
//	bool	load(const string& modelPath, const string& animPath, const string& texPath);

	static Entity* create(const EntityData& data);
	bool	load(const EntityData&	animFile);

	void	update(float dt);

	//Base
	Sprite3D*	getSprite3d()const				{ return _Sprite3d; }
	const AnimMap&	getAnimMap()const			{ return _AnimList; }
	const std::string& getFriendlyName()const	{ return _name; }
	const std::string& getModelName()const		{ return _modelName; }
	
	//Animation
	const std::string& getCurrAnimName()const	{ return _currAnim->first; }
	int		getFramesCount()const					{ return _TotalFrames; }
	void	switchAnim(int step);
	void	switchAnim(const std::string& animName);
	void	recreateCurrentAnim(int from, int to);
	void	addNewAnimSection(const std::string& newAnimName);
	bool	removeCurrentAnim();

	//Debug
	void	setDrawingBoundingBox(bool state);

protected:
	void parseAnimSection(const EntityData& animFile,Animation3D* anim);

private:
	RefPtr<Sprite3D>	_Sprite3d;

	std::string			_name;
	std::string			_modelName;

	//Anim
	int					_TotalFrames;
	AnimMap				_AnimList;
	AnimMapIter			_currAnim;

	//For debaug
	RefPtr<DrawNode3D>  _drawDebug;
	OBB					_obbt;
};

#endif
#ifndef __ANIM_INDEX_FILE_PARSER_H__
#define __ANIM_INDEX_FILE_PARSER_H__

#include "cocos2d.h"

using std::string;

struct ResData{
	string name;
	string modelFile;
	string texFile;
	string animFile;

	struct AnimFrames{
		AnimFrames(const string& n, int f, int e) :name(n), start(f), end(e)	{}
		AnimFrames() :start(0), end(0)	{}
		string name;
		int start;
		int end;
	};
	std::vector<AnimFrames>	animList;

};
typedef std::vector<ResData>	ResourceDataList;


class IndexFileParser{
protected:
	IndexFileParser();
	~IndexFileParser();

public:
	static ResData*	loadNewModel(const std::string& filePath, const std::string& text = "");

	static ResourceDataList*	parseIndexFile(const std::string&	filePath);
	static bool					serializeToFile();

	static ResData::AnimFrames*	findAnim(const std::string&	modelName, const std::string& animName);
	static ResData*				findViewDate(const std::string& modelName);

	static ResourceDataList		s_AnimFileData;
	static const float			sFrameRate;
	static const std::string	s_DefaultAnim;

private:
	static bool	_InSerializing;

	static std::string	_DataFileName;
};

#endif
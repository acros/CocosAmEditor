#include "DataHandler.h"
#include "json/reader.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include <fstream>

USING_NS_CC;

const float DataHandler::sFrameRate = 30.f;
const std::string DataHandler::s_DefaultAnim = "Default Animation";
std::string DataHandler::_DataFileName = "config.json";

bool DataHandler::_InSerializing = false;

ResourceDataList DataHandler::s_AnimFileData;

ResourceDataList* DataHandler::deserializeFromFile(const std::string& filePath)
{
	_DataFileName = filePath;

	std::string contentStr = FileUtils::getInstance()->getStringFromFile(filePath);

	rapidjson::Document doc;
	doc.Parse<0>(contentStr.c_str());

	if ( doc.HasParseError() || !doc.HasMember("data"))
		return nullptr;

	s_AnimFileData.clear();

	rapidjson::Value&	na = doc["data"];
	int nodeSize = na.Size();
	for (int i = 0; i < nodeSize; ++i)
	{
		rapidjson::Value& nodeValue = na[i];

		//Parse node context
		EntityData	t;
		if (nodeValue.HasMember("model")){
			t.modelFile = nodeValue["model"].GetString();
		}

		if (nodeValue.HasMember("name")) {
			t.name = nodeValue["name"].GetString();
		}
		if (nodeValue.HasMember("tex")){
			t.texFile = nodeValue["tex"].GetString();
		}
		if (nodeValue.HasMember("anim")){
			t.animFile = nodeValue["anim"].GetString();
		}else{
			t.animFile = t.modelFile;
		}

// 		if (nodeValue.HasMember("sec") && nodeValue["sec"].IsArray()){
// 			rapidjson::Value& secValue = nodeValue["sec"][0u];
// 			for (auto itr = secValue.MemberonBegin(); itr != secValue.MemberonEnd(); ++itr){
// 				ResourceData::AnimFrames secFrame;
// 				secFrame.name = itr->name.GetString();
// 				if (itr->value.IsArray()){
// 					secFrame.start = itr->value[0u].GetInt();
// 					secFrame.end = itr->value[1].GetInt();
// 				}
// 				t.animList.push_back(secFrame);
// 			}
// 		}

		if (nodeValue.HasMember("sec") && nodeValue["sec"].IsObject()){
			rapidjson::Value& secValue = nodeValue["sec"];
			for (auto itr = secValue.MemberBegin(); itr != secValue.MemberEnd(); ++itr){
				EntityData::AnimFrames secFrame;
				secFrame.name = itr->name.GetString();
				if (itr->value.IsArray()){
					secFrame.start = itr->value[0u].GetInt();
					secFrame.end = itr->value[1].GetInt();
				}
				t.animList.push_back(secFrame);
			}
		}


		s_AnimFileData.push_back(t);
	}

	return &s_AnimFileData;
}

EntityData::AnimFrames* DataHandler::findAnim(const std::string& modelName, const std::string& animName)
{
	auto itr = s_AnimFileData.begin();
	for (; itr != s_AnimFileData.end(); ++itr)
	{
		if (itr->name == modelName)
			break;
	}

	if (itr != s_AnimFileData.end())
	{
		for (auto animItr = itr->animList.begin(); animItr != itr->animList.end(); ++animItr){
			if (animItr->name == animName)
				return &(*animItr);
		}
	}

	return nullptr;
}

EntityData* DataHandler::findViewDate(const std::string& modelName)
{
	for (auto itr = s_AnimFileData.begin(); itr != s_AnimFileData.end(); ++itr)
	{
		if (itr->name == modelName)
			return &(*itr);
	}

	return nullptr;
}


EntityData* DataHandler::loadNewModel(const std::string& filePath, const std::string& animPath, const std::string& tex /*= ""*/)
{
	EntityData newData;
	newData.name = filePath;

	FileUtils::getInstance()->addSearchPath("data/" + filePath);
	if (FileUtils::getInstance()->isFileExist(filePath)){
		newData.modelFile = filePath;
	}
	else if (FileUtils::getInstance()->isFileExist(filePath + ".c3t")){
		newData.modelFile = filePath + ".c3t";
	}
	else if (FileUtils::getInstance()->isFileExist(filePath + ".c3b")){
		newData.modelFile = filePath + ".c3b";
	}

	if (!tex.empty()){
		newData.texFile = tex;
	}

	if (newData.modelFile.empty())
		return nullptr;

	if (animPath.empty())
		newData.animFile = newData.modelFile;
	else 
		newData.animFile = animPath;

	s_AnimFileData.push_back(newData);

	return &(s_AnimFileData.back());
}

bool DataHandler::serializeToFile()
{
	if (_InSerializing)
		return false;

	_InSerializing = true;

	//////////////////////////////////////////////////////////////////////////

	//Serialize to json
	rapidjson::Document Doc;
	Doc.SetObject();

	rapidjson::Document::AllocatorType& allocator = Doc.GetAllocator();
	rapidjson::Value vElem(rapidjson::kArrayType);
	for (rapidjson::SizeType i = 0; i < s_AnimFileData.size(); ++i)
	{
		typedef rapidjson::Value::StringRefType rjStr;

		rapidjson::Value vElemItem(rapidjson::kObjectType);
		vElemItem.AddMember(rjStr("name"), rjStr(s_AnimFileData.at(i).name.c_str()), allocator);

		vElemItem.AddMember(rjStr("model"), rjStr(s_AnimFileData.at(i).modelFile.c_str()), allocator);
		if (!s_AnimFileData.at(i).texFile.empty()){
			vElemItem.AddMember(rjStr("tex"), rjStr(s_AnimFileData.at(i).texFile.c_str()), allocator);
		}
		if (!s_AnimFileData.at(i).animFile.empty()){
			vElemItem.AddMember(rjStr("anim"), rjStr(s_AnimFileData.at(i).modelFile.c_str()), allocator);
		}

		if (s_AnimFileData.at(i).animList.size() > 0){
			rapidjson::Value vAnimElemItem(rapidjson::kObjectType);
			for (auto& animElement : s_AnimFileData.at(i).animList){
				rapidjson::Value vAnimFrameSec(rapidjson::kArrayType);
				vAnimFrameSec.PushBack(animElement.start, allocator);
				vAnimFrameSec.PushBack(animElement.end, allocator);
				vAnimElemItem.AddMember(rjStr(animElement.name.c_str()), vAnimFrameSec, allocator);
			}
			vElemItem.AddMember("sec", vAnimElemItem, allocator);
		}

		vElem.PushBack(vElemItem, allocator);
	}
	Doc.AddMember("data", vElem, allocator);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	Doc.Accept(writer);
	std::string strJson(buffer.GetString(), buffer.GetSize());

	auto outputFile = FileUtils::getInstance()->fullPathForFilename(_DataFileName);

//	std::string filepath = (CCFileUtils::sharedFileUtils()->getWritablePath() + "test.json");
	std::ofstream outfile;
	outfile.open(outputFile.c_str());
	if (outfile.fail())
	{
		return	false;
	}
	outfile << strJson;
	outfile.close();

	//////////////////////////////////////////////////////////////////////////

	_InSerializing = false;

	return true;
}



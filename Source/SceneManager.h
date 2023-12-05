#pragma once
#include <map>
#include <string>
#include "SceneTitle.h"


class SceneManager
{
private:

	SceneManager() {};
	~SceneManager() {};
	std::string nextScene = "";
	std::map<std::string, Scene*> scenes;


public:
	static SceneManager& getInstance()
	{
		static SceneManager inst;
		return inst;
	}
	Scene* curScene = nullptr;


	void Initialize();
	void Finalize();
	void changeScene();
};

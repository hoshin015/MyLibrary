#include "SceneManager.h"



void SceneManager::Initialize()
{
	scenes["title"] = new SceneTitle;

	nextScene = "title";
}

void SceneManager::Finalize()
{
	for (auto& scene : scenes)
	{
		delete &scene;
	}
}

void SceneManager::changeScene()
{
	if (nextScene.empty()) return;

	// シーンの終了処理
	if(curScene != nullptr)
		curScene->Finalize();

	// シーン切り替え
	curScene = scenes[nextScene];

	// シーン開始処理
	curScene->Initialize();

	nextScene.clear();
}

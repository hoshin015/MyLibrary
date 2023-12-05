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

	// �V�[���̏I������
	if(curScene != nullptr)
		curScene->Finalize();

	// �V�[���؂�ւ�
	curScene = scenes[nextScene];

	// �V�[���J�n����
	curScene->Initialize();

	nextScene.clear();
}

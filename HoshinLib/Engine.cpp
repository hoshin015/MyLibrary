#include "Engine.h"
#include "../Source/SceneManager.h"

bool Engine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	if (!this->render_window.Initialize(this, hInstance, window_title, window_class, width, height))
		return false;

	if (!gfx.Initialize(this->render_window.GetHWND(), width, height))
		return false;

	SceneManager::getInstance().Initialize();

	return true;
}

bool Engine::ProcessMessages()
{
	return this->render_window.ProcessMessages();
}

void Engine::Update()
{
	// 入力された文字の取得
	while (!keyboard.CharBufferIsEmpty())
	{
		unsigned char ch = keyboard.ReadChar();
	}

	// 入力されたキーの取得
	while (!keyboard.KeyBufferIsEmpty())
	{
		KeyboardEvent kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
	}

	// マウスに関する取得
	while (!mouse.EventBufferIsEmpty())
	{
		MouseEvent me = mouse.ReadEvent();
	}

	// シーンチェンジチェック
	SceneManager::getInstance().changeScene();

	// シーンの更新
	SceneManager::getInstance().curScene->Update();
}

void Engine::RenderFrame()
{
	gfx.RenderFrame();

	SceneManager::getInstance().curScene->Render();
}

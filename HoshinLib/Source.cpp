#include "Engine.h"

int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,			// このアプリに割り振られたインスタンスハンドル
	_In_opt_ HINSTANCE hPrevInstance,	// 旧仕様の残り（常にnullptrが代入されている）
	_In_ LPWSTR		lpCmdLine,			// コマンドライン引数が格納された文字列
	_In_ int		nCmdShow			// ウィンドウ初期状態の指定
)
{
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to call CoInitialize.");
		return -1;
	}

	Engine engine;
	// 初期化(ウィンドウの生成・DirectXの初期化)
	if (engine.Initialize(hInstance, "Title", "MyWindowClass", 800, 600)) 
	{
		while (engine.ProcessMessages())	// このタイミングでメッセージの取得(WindowProcの処理も多分これのすぐ後)
		{
			engine.Update();
			engine.RenderFrame();
		}
	}

	return 0;
}
#include "WindowContainer.h"

/// <summary>
/// クラスのメンバ変数を設定する
/// </summary>
/// <param name="hInstance"></param>
/// <param name="window_title"></param>
/// <param name="window_class"></param>
/// <param name="width"></param>
/// <param name="height"></param>
/// <returns></returns>
bool RenderWindow::Initialize(WindowContainer* pWindowContainer, HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	this->hInstance = hInstance;
	this->width = width;
	this->height = height;
	this->window_title = window_title;
	this->window_title_wide = StringConverter::StringToWide(this->window_title);
	this->window_class = window_class;
	this->window_class_wide = StringConverter::StringToWide(this->window_class);

	// ウィンドウクラスの登録
	this->RegisterWindowClass();


	// ウィンドウ表示地点の取得(画面中央)
	int centerScreenX = GetSystemMetrics(SM_CXSCREEN) / 2 - this->width / 2;
	int centerScreenY = GetSystemMetrics(SM_CYSCREEN) / 2 - this->height / 2;

	// ウィンドウサイズの調整
	RECT wr; 
	wr.left = centerScreenX;
	wr.top = centerScreenY;
	wr.right = wr.left + this->width;
	wr.bottom = wr.top + this->height;
	AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);


	// ウィンドウの作成
	this->handle = CreateWindowEx(	// ウィンドウが正常に作成されなかった場合はNULLを返す
		0,					// 拡張ウィンドウスタイル
		this->window_class_wide.c_str(),			// ウィンドウクラス名
		this->window_title_wide.c_str(),			// ウィンドウタイトル名
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,	// ウィンドウスタイル
		wr.left,				// ウィンドウのX座標
		wr.top,					// ウィンドウのY座標
		wr.right - wr.left,		// ウィンドウの幅
		wr.bottom - wr.top,		// ウィンドウの高さ
		NULL,				// このウィンドウの親へのハンドル（最初のウィンドウのため親ウィンドウはない）
		NULL,				// メニューまたは子ウィンドウの識別子へのハンドル（メニューを使用したい場合はNULLに設定し、WindowClassExでmenuを使用することができる）
		this->hInstance,	// このウィンドウで使用するモジュールのインスタンスへのハンドル
		pWindowContainer	// ウィンドウを作成するパラメータ
	);

	// ウィンドウが正常に作成されたかのチェック
	if (this->handle == NULL)
	{
		ErrorLogger::Log(GetLastError(), "CreateWindowEX Failed for window: " + this->window_title);
		return false;
	}

	// ウィンドウを画面上に表示し、メインフォーカスに設定する
	ShowWindow(this->handle, SW_SHOW);	// windowを表示状態にする
	SetForegroundWindow(this->handle);
	SetFocus(this->handle);


	return true;
}

bool RenderWindow::ProcessMessages()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));	// メッセージ構造体を初期化

	// メッセージの取得（メッセージの有無に関係なく処理は進む）
	while (PeekMessage(	// 戻り値：メッセージの取得結果（成功：true　失敗:false）
		&msg,			// メッセージ情報を受信するMSG構造体へのポインター
		this->handle,	// メッセージを取得するウィンドウへのハンドル
		0,				// 検査するメッセージの範囲内の最初のメッセージの値
		0,				// 検査するメッセージの範囲内の最後のメッセージの値
		PM_REMOVE		// メッセージの処理方法（ここでは同じメッセージを２回読み込まないようにメッセージを削除している）
		))
	{
		TranslateMessage(&msg);		// 仮想キーメッセージを文字メッセージに変換
		DispatchMessage(&msg);		// メッセージをウィンドウプロシージャに送信（ウィンドウプロシージャを呼び出す処理みたいなイメージだと思う）
	}

	// ウィンドウが閉じられたかどうかチェック
	if (msg.message == WM_NULL)
	{
		if (!IsWindow(this->handle))
		{
			this->handle = NULL;	// メッセージ処理ループがこのウィンドウの破棄を行う
			UnregisterClass(this->window_class_wide.c_str(), this->hInstance);	// ウィンドウクラスの登録を解除し、 クラスに必要なメモリを解放する
			return false;	// source.cppのループを止める
		}
	}

	return true;	// source.cppのループを続ける
}

HWND RenderWindow::GetHWND() const
{
	return this->handle;
}

RenderWindow::~RenderWindow()
{
	if (this->handle != NULL)
	{
		UnregisterClass(this->window_class_wide.c_str(), this->hInstance);	// ウィンドウクラスの登録を解除し、 クラスに必要なメモリを解放する
		DestroyWindow(handle);
	}
}

LRESULT CALLBACK HandleMsgRedirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		// その他すべてのメッセージ
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;

	default:
	{
		// ウィンドウクラスへのptrを取得する
		WindowContainer* const pWindow = reinterpret_cast<WindowContainer*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		// メッセージをウィンドウ クラス ハンドラに転送する
		return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
	}
	}
}


/// <summary>
/// OS(Windows)から送信されるメッセージを処理するためのコールバック
/// </summary>
/// <param name="hwnd">WINAPI関連のCALLBACK関数に必要な規約</param>
/// <param name="uMsg">メッセージに対する処理を行ったか(行った：０　行っていない：DefWindowProcの戻り値)</param>
/// <param name="wParam">送信されたメッセージ</param>
/// <param name="lParam">メッセージに関する付加情報</param>
/// <returns></returns>
LRESULT CALLBACK HandleMsgSetup(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_NCCREATE:	// ウィンドウが最初に作成された時
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		WindowContainer* pWindow = reinterpret_cast<WindowContainer*>(pCreate->lpCreateParams);
		if (pWindow == nullptr)
		{
			ErrorLogger::Log("Critical Error: Pointer to window container is null during WM_NCCREATE.");
			exit(-1);
		}
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMsgRedirect));
		return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	
}


/// <summary>
/// ウィンドウクラスの登録
/// 
/// ウィンドウの作成には数多くの情報が必要だが、それをすべてCreateWindow関数で行うと
/// 引数の数が膨大になるため事前に情報を設定しておく
/// </summary>
void RenderWindow::RegisterWindowClass()
{
	WNDCLASSEX wc;	//ウィンドウクラス情報の構造体
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;		// クラススタイルの組み合わせ(ウィンドウの幅や高さ等を変更した際に再描画する等？)
	wc.lpfnWndProc = HandleMsgSetup;					// ウィンドウプロシージャのアドレス
	wc.cbClsExtra = 0;									// 補助メモリ
	wc.cbWndExtra = 0;									// 補助メモリ
	wc.hInstance = this->hInstance;						// このアプリのインスタンスハンドル
	wc.hIcon = NULL;									// アプリのショートカット等で仕様されるアイコンを指定できる
	wc.hIconSm = NULL;									// タイトルバーで使用されるアイコンの指定
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// ウィンドウ上のマウスカーソルを指定できる
	wc.hbrBackground = NULL;							// ウィンドウのクライアント領域の背景色
	wc.lpszMenuName = NULL;								// ウィンドウ上段に設定するメニューの名前（メニューがなければNULL）
	wc.lpszClassName = this->window_class_wide.c_str();	// 登録時に使用するウィンドウクラスの名前
	wc.cbSize = sizeof(WNDCLASSEX);						// 構造体のサイズ
	RegisterClassExW(&wc);								// 情報の登録
}
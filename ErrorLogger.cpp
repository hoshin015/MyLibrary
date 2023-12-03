#include "ErrorLogger.h"
#include <comdef.h>

void ErrorLogger::Log(std::string message)
{
	std::string error_message = "Error: " + message;	// エラーメッセージの作成
	
	// ダイアログボックスの生成
	MessageBoxA(
		NULL,					// 所有者ウィンドウへのハンドル
		error_message.c_str(),	// 表示するメッセージ
		"Error",				// ダイアログボックスのタイトル
		MB_ICONERROR			// ダイアログボックスの内容と動作
	);
}

void ErrorLogger::Log(HRESULT hr, std::string message)
{
	_com_error error(hr);
	std::wstring error_message = L"Error: " + StringConverter::StringToWide(message) + L"\n" + error.ErrorMessage();
	
	MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
}

void ErrorLogger::Log(HRESULT hr, std::wstring message)
{
	_com_error error(hr);
	std::wstring error_message = L"Error: " + message + L"\n" + error.ErrorMessage();

	MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
}

#include "ErrorLogger.h"
#include <comdef.h>

void ErrorLogger::Log(std::string message)
{
	std::string error_message = "Error: " + message;	// �G���[���b�Z�[�W�̍쐬
	
	// �_�C�A���O�{�b�N�X�̐���
	MessageBoxA(
		NULL,					// ���L�҃E�B���h�E�ւ̃n���h��
		error_message.c_str(),	// �\�����郁�b�Z�[�W
		"Error",				// �_�C�A���O�{�b�N�X�̃^�C�g��
		MB_ICONERROR			// �_�C�A���O�{�b�N�X�̓��e�Ɠ���
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

#include "Engine.h"

int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,			// ���̃A�v���Ɋ���U��ꂽ�C���X�^���X�n���h��
	_In_opt_ HINSTANCE hPrevInstance,	// ���d�l�̎c��i���nullptr���������Ă���j
	_In_ LPWSTR		lpCmdLine,			// �R�}���h���C���������i�[���ꂽ������
	_In_ int		nCmdShow			// �E�B���h�E������Ԃ̎w��
)
{
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to call CoInitialize.");
		return -1;
	}

	Engine engine;
	// ������(�E�B���h�E�̐����EDirectX�̏�����)
	if (engine.Initialize(hInstance, "Title", "MyWindowClass", 800, 600)) 
	{
		while (engine.ProcessMessages())	// ���̃^�C�~���O�Ń��b�Z�[�W�̎擾(WindowProc�̏�������������̂�����)
		{
			engine.Update();
			engine.RenderFrame();
		}
	}

	return 0;
}
#include "WindowContainer.h"

/// <summary>
/// �N���X�̃����o�ϐ���ݒ肷��
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

	// �E�B���h�E�N���X�̓o�^
	this->RegisterWindowClass();


	// �E�B���h�E�\���n�_�̎擾(��ʒ���)
	int centerScreenX = GetSystemMetrics(SM_CXSCREEN) / 2 - this->width / 2;
	int centerScreenY = GetSystemMetrics(SM_CYSCREEN) / 2 - this->height / 2;

	// �E�B���h�E�T�C�Y�̒���
	RECT wr; 
	wr.left = centerScreenX;
	wr.top = centerScreenY;
	wr.right = wr.left + this->width;
	wr.bottom = wr.top + this->height;
	AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);


	// �E�B���h�E�̍쐬
	this->handle = CreateWindowEx(	// �E�B���h�E������ɍ쐬����Ȃ������ꍇ��NULL��Ԃ�
		0,					// �g���E�B���h�E�X�^�C��
		this->window_class_wide.c_str(),			// �E�B���h�E�N���X��
		this->window_title_wide.c_str(),			// �E�B���h�E�^�C�g����
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,	// �E�B���h�E�X�^�C��
		wr.left,				// �E�B���h�E��X���W
		wr.top,					// �E�B���h�E��Y���W
		wr.right - wr.left,		// �E�B���h�E�̕�
		wr.bottom - wr.top,		// �E�B���h�E�̍���
		NULL,				// ���̃E�B���h�E�̐e�ւ̃n���h���i�ŏ��̃E�B���h�E�̂��ߐe�E�B���h�E�͂Ȃ��j
		NULL,				// ���j���[�܂��͎q�E�B���h�E�̎��ʎq�ւ̃n���h���i���j���[���g�p�������ꍇ��NULL�ɐݒ肵�AWindowClassEx��menu���g�p���邱�Ƃ��ł���j
		this->hInstance,	// ���̃E�B���h�E�Ŏg�p���郂�W���[���̃C���X�^���X�ւ̃n���h��
		pWindowContainer	// �E�B���h�E���쐬����p�����[�^
	);

	// �E�B���h�E������ɍ쐬���ꂽ���̃`�F�b�N
	if (this->handle == NULL)
	{
		ErrorLogger::Log(GetLastError(), "CreateWindowEX Failed for window: " + this->window_title);
		return false;
	}

	// �E�B���h�E����ʏ�ɕ\�����A���C���t�H�[�J�X�ɐݒ肷��
	ShowWindow(this->handle, SW_SHOW);	// window��\����Ԃɂ���
	SetForegroundWindow(this->handle);
	SetFocus(this->handle);


	return true;
}

bool RenderWindow::ProcessMessages()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));	// ���b�Z�[�W�\���̂�������

	// ���b�Z�[�W�̎擾�i���b�Z�[�W�̗L���Ɋ֌W�Ȃ������͐i�ށj
	while (PeekMessage(	// �߂�l�F���b�Z�[�W�̎擾���ʁi�����Ftrue�@���s:false�j
		&msg,			// ���b�Z�[�W������M����MSG�\���̂ւ̃|�C���^�[
		this->handle,	// ���b�Z�[�W���擾����E�B���h�E�ւ̃n���h��
		0,				// �������郁�b�Z�[�W�͈͓̔��̍ŏ��̃��b�Z�[�W�̒l
		0,				// �������郁�b�Z�[�W�͈͓̔��̍Ō�̃��b�Z�[�W�̒l
		PM_REMOVE		// ���b�Z�[�W�̏������@�i�����ł͓������b�Z�[�W���Q��ǂݍ��܂Ȃ��悤�Ƀ��b�Z�[�W���폜���Ă���j
		))
	{
		TranslateMessage(&msg);		// ���z�L�[���b�Z�[�W�𕶎����b�Z�[�W�ɕϊ�
		DispatchMessage(&msg);		// ���b�Z�[�W���E�B���h�E�v���V�[�W���ɑ��M�i�E�B���h�E�v���V�[�W�����Ăяo�������݂����ȃC���[�W���Ǝv���j
	}

	// �E�B���h�E������ꂽ���ǂ����`�F�b�N
	if (msg.message == WM_NULL)
	{
		if (!IsWindow(this->handle))
		{
			this->handle = NULL;	// ���b�Z�[�W�������[�v�����̃E�B���h�E�̔j�����s��
			UnregisterClass(this->window_class_wide.c_str(), this->hInstance);	// �E�B���h�E�N���X�̓o�^���������A �N���X�ɕK�v�ȃ��������������
			return false;	// source.cpp�̃��[�v���~�߂�
		}
	}

	return true;	// source.cpp�̃��[�v�𑱂���
}

HWND RenderWindow::GetHWND() const
{
	return this->handle;
}

RenderWindow::~RenderWindow()
{
	if (this->handle != NULL)
	{
		UnregisterClass(this->window_class_wide.c_str(), this->hInstance);	// �E�B���h�E�N���X�̓o�^���������A �N���X�ɕK�v�ȃ��������������
		DestroyWindow(handle);
	}
}

LRESULT CALLBACK HandleMsgRedirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		// ���̑����ׂẴ��b�Z�[�W
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;

	default:
	{
		// �E�B���h�E�N���X�ւ�ptr���擾����
		WindowContainer* const pWindow = reinterpret_cast<WindowContainer*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		// ���b�Z�[�W���E�B���h�E �N���X �n���h���ɓ]������
		return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
	}
	}
}


/// <summary>
/// OS(Windows)���瑗�M����郁�b�Z�[�W���������邽�߂̃R�[���o�b�N
/// </summary>
/// <param name="hwnd">WINAPI�֘A��CALLBACK�֐��ɕK�v�ȋK��</param>
/// <param name="uMsg">���b�Z�[�W�ɑ΂��鏈�����s������(�s�����F�O�@�s���Ă��Ȃ��FDefWindowProc�̖߂�l)</param>
/// <param name="wParam">���M���ꂽ���b�Z�[�W</param>
/// <param name="lParam">���b�Z�[�W�Ɋւ���t�����</param>
/// <returns></returns>
LRESULT CALLBACK HandleMsgSetup(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_NCCREATE:	// �E�B���h�E���ŏ��ɍ쐬���ꂽ��
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
/// �E�B���h�E�N���X�̓o�^
/// 
/// �E�B���h�E�̍쐬�ɂ͐������̏�񂪕K�v�����A��������ׂ�CreateWindow�֐��ōs����
/// �����̐����c��ɂȂ邽�ߎ��O�ɏ���ݒ肵�Ă���
/// </summary>
void RenderWindow::RegisterWindowClass()
{
	WNDCLASSEX wc;	//�E�B���h�E�N���X���̍\����
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;		// �N���X�X�^�C���̑g�ݍ��킹(�E�B���h�E�̕��⍂������ύX�����ۂɍĕ`�悷�铙�H)
	wc.lpfnWndProc = HandleMsgSetup;					// �E�B���h�E�v���V�[�W���̃A�h���X
	wc.cbClsExtra = 0;									// �⏕������
	wc.cbWndExtra = 0;									// �⏕������
	wc.hInstance = this->hInstance;						// ���̃A�v���̃C���X�^���X�n���h��
	wc.hIcon = NULL;									// �A�v���̃V���[�g�J�b�g���Ŏd�l�����A�C�R�����w��ł���
	wc.hIconSm = NULL;									// �^�C�g���o�[�Ŏg�p�����A�C�R���̎w��
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// �E�B���h�E��̃}�E�X�J�[�\�����w��ł���
	wc.hbrBackground = NULL;							// �E�B���h�E�̃N���C�A���g�̈�̔w�i�F
	wc.lpszMenuName = NULL;								// �E�B���h�E��i�ɐݒ肷�郁�j���[�̖��O�i���j���[���Ȃ����NULL�j
	wc.lpszClassName = this->window_class_wide.c_str();	// �o�^���Ɏg�p����E�B���h�E�N���X�̖��O
	wc.cbSize = sizeof(WNDCLASSEX);						// �\���̂̃T�C�Y
	RegisterClassExW(&wc);								// ���̓o�^
}
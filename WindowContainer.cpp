#include "WindowContainer.h"
#include <memory>

WindowContainer::WindowContainer()
{
	// raw input �̎g�p�����H
	static bool raw_input_initialized = false;
	if (raw_input_initialized == false)
	{
		RAWINPUTDEVICE rid;

		rid.usUsagePage = 0x01;
		rid.usUsage = 0x02;
		rid.dwFlags = 0;
		rid.hwndTarget = NULL;

		if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
		{
			ErrorLogger::Log(GetLastError(), "Failed to register raw input devices.");
			exit(-1);
		}

		raw_input_initialized = true;
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
LRESULT WindowContainer::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:	// �L�[�������ꂽ�Ƃ��̃L�[�R�[�h�������Ă���
	{
		unsigned char keycode = static_cast<unsigned char>(wParam);
		if (keyboard.IsKeysAutoRepeat())
		{
			keyboard.OnKeyPressed(keycode);
		}
		else
		{
			const bool wasPressed = lParam & 0x40000000;
			if (!wasPressed)
			{
				keyboard.OnKeyPressed(keycode);
			}
		}
		return 0;
	}
	case WM_KEYUP:	// �L�[�������ꂽ�Ƃ��̃L�[�R�[�h�������Ă���
	{
		unsigned char keycode = static_cast<unsigned char>(wParam);
		keyboard.OnKeyReleased(keycode);
		return 0;
	}
	case WM_CHAR:	// �����ꂽ�����̃L�[�R�[�h�������Ă���
	{
		unsigned char ch = static_cast<unsigned char>(wParam);
		if (keyboard.IsCharsAutoRepeat())
		{
			keyboard.OnChar(ch);
		}
		else
		{
			const bool wasPressed = lParam & 0x40000000;
			if (!wasPressed)
			{
				keyboard.OnChar(ch);
			}
		}
		return 0;
	}
	case WM_MOUSEMOVE:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.OnMouseMove(x, y);
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.OnLeftPressed(x, y);
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.OnRightPressed(x, y);
		return 0;
	}
	case WM_MBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.OnMiddlePressed(x, y);
		return 0;
	}
	case WM_LBUTTONUP:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.OnLeftReleased(x, y);
		return 0;
	}
	case WM_RBUTTONUP:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.OnRightReleased(x, y);
		return 0;
	}
	case WM_MBUTTONUP:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.OnMiddleReleased(x, y);
		return 0;
	}
	case WM_MOUSEWHEEL:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
		{
			mouse.OnWheelUp(x, y);
		}
		else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
		{
			mouse.OnWheelDown(x, y);
		}
		break;
	}
	case WM_INPUT:
	{
		UINT dataSize;

		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));
		if (dataSize > 0)
		{
			std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);
			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawdata.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
			{
				RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.get());
				if (raw->header.dwType == RIM_TYPEMOUSE)
				{
					mouse.OnMouseMoveRaw(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
				}
			}
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	
}

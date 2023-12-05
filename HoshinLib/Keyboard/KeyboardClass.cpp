#include "KeyboardClass.h"

// �L�[�̏�Ԃ�false�ɏ���������
KeyboardClass::KeyboardClass()
{
	for (int i = 0; i < 256; i++)
	{
		this->keyStates[i] = false;
	}
}

bool KeyboardClass::KeyIsPressed(const unsigned char keycode)
{
	return this->keyStates[keycode];
}

bool KeyboardClass::KeyBufferIsEmpty()
{
	return this->keyBuffer.empty();
}

bool KeyboardClass::CharBufferIsEmpty()
{
	return this->charBuffer.empty();
}

KeyboardEvent KeyboardClass::ReadKey()
{
	if (this->keyBuffer.empty())	// �L�[�o�b�t�@����̏ꍇ
	{
		return KeyboardEvent();		//�����ȃL�[�C�x���g��Ԃ�
	}
	else
	{
		// �L�[�o�b�t�@�̒��g������ꍇ�͐擪�̃C�x���g��pop���ĕԂ�
		KeyboardEvent e = this->keyBuffer.front();
		this->keyBuffer.pop();
		return e;
	}
}

unsigned char KeyboardClass::ReadChar()
{
	if (this->charBuffer.empty())	//�����o�b�t�@����̏ꍇ
	{
		return 0u;					// 0(NULL char)��Ԃ�
	}
	else
	{
		// �����o�b�t�@�̒��g������ꍇ�͐擪�̕�����pop���ĕԂ�
		unsigned char e = this->charBuffer.front();
		this->charBuffer.pop();
		return e;
	}
}

void KeyboardClass::OnKeyPressed(const unsigned char key)
{
	//	�L�[�{�[�hPress���L���[�Ƀv�b�V������
	this->keyStates[key] = true;
	this->keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Press, key));
}

void KeyboardClass::OnKeyReleased(const unsigned char key)
{
	// �L�[�{�[�hRelease���L���[�Ƀv�b�V������
	this->keyStates[key] = false;
	this->keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Release, key));
}

void KeyboardClass::OnChar(const unsigned char key)
{
	this->charBuffer.push(key);
}

// �������s�[�g�L�[��L����
void KeyboardClass::EnableAutoRepeatKeys()
{
	this->autoRepeatKeys = true;
}

// �������s�[�g�L�[�𖳌���
void KeyboardClass::DisableAutoRepeatKeys()
{
	this->autoRepeatKeys = false;
}

// �������s�[�g������L����
void KeyboardClass::EnableAutoRepeatChars()
{
	this->autoRepeatChars = true;
}

// �������s�[�g�����𖳌���
void KeyboardClass::DisableAutoRepeatChars()
{
	this->autoRepeatChars = false;
}

bool KeyboardClass::IsKeysAutoRepeat()
{
	return this->autoRepeatKeys;
}

bool KeyboardClass::IsCharsAutoRepeat()
{
	return this->autoRepeatChars;
}
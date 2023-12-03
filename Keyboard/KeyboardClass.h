#pragma once
#include "KeyboardEvent.h"
#include <queue>

class KeyboardClass
{
public:
	KeyboardClass();
	bool KeyIsPressed(const unsigned char keycode);	// �L�[�������ꂽ���ǂ���
	bool KeyBufferIsEmpty();						// �L�[�o�b�t�@���󂩂ǂ���
	bool CharBufferIsEmpty();						// �����o�b�t�@���󂩂ǂ���
	KeyboardEvent ReadKey();
	unsigned char ReadChar();
	void OnKeyPressed(const unsigned char key);		// �L�[�������ꂽ�Ƃ��E�B���h�E�v���V�[�W���ŌĂяo��
	void OnKeyReleased(const unsigned char key);	// �L�[�������ꂽ�Ƃ��E�B���h�E�v���V�[�W���ŌĂяo��
	void OnChar(const unsigned char key);			// �����������ꂽ�Ƃ��E�B���h�E�v���V�[�W���ŌĂяo��
	void EnableAutoRepeatKeys();
	void DisableAutoRepeatKeys();
	void EnableAutoRepeatChars();
	void DisableAutoRepeatChars();
	bool IsKeysAutoRepeat();
	bool IsCharsAutoRepeat();

private:
	bool autoRepeatKeys = false;
	bool autoRepeatChars = false;
	bool keyStates[256];
	std::queue<KeyboardEvent> keyBuffer;
	std::queue<unsigned char> charBuffer;
};
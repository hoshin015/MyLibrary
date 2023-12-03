#include "KeyboardClass.h"

// キーの状態をfalseに初期化する
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
	if (this->keyBuffer.empty())	// キーバッファが空の場合
	{
		return KeyboardEvent();		//無効なキーイベントを返す
	}
	else
	{
		// キーバッファの中身がある場合は先頭のイベントをpopして返す
		KeyboardEvent e = this->keyBuffer.front();
		this->keyBuffer.pop();
		return e;
	}
}

unsigned char KeyboardClass::ReadChar()
{
	if (this->charBuffer.empty())	//文字バッファが空の場合
	{
		return 0u;					// 0(NULL char)を返す
	}
	else
	{
		// 文字バッファの中身がある場合は先頭の文字をpopして返す
		unsigned char e = this->charBuffer.front();
		this->charBuffer.pop();
		return e;
	}
}

void KeyboardClass::OnKeyPressed(const unsigned char key)
{
	//	キーボードPressをキューにプッシュする
	this->keyStates[key] = true;
	this->keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Press, key));
}

void KeyboardClass::OnKeyReleased(const unsigned char key)
{
	// キーボードReleaseをキューにプッシュする
	this->keyStates[key] = false;
	this->keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Release, key));
}

void KeyboardClass::OnChar(const unsigned char key)
{
	this->charBuffer.push(key);
}

// 自動リピートキーを有効化
void KeyboardClass::EnableAutoRepeatKeys()
{
	this->autoRepeatKeys = true;
}

// 自動リピートキーを無効化
void KeyboardClass::DisableAutoRepeatKeys()
{
	this->autoRepeatKeys = false;
}

// 自動リピート文字を有効化
void KeyboardClass::EnableAutoRepeatChars()
{
	this->autoRepeatChars = true;
}

// 自動リピート文字を無効化
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
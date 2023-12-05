#pragma once
#include "KeyboardEvent.h"
#include <queue>

class KeyboardClass
{
public:
	KeyboardClass();
	bool KeyIsPressed(const unsigned char keycode);	// キーが押されたかどうか
	bool KeyBufferIsEmpty();						// キーバッファが空かどうか
	bool CharBufferIsEmpty();						// 文字バッファが空かどうか
	KeyboardEvent ReadKey();
	unsigned char ReadChar();
	void OnKeyPressed(const unsigned char key);		// キーが押されたときウィンドウプロシージャで呼び出す
	void OnKeyReleased(const unsigned char key);	// キーが離されたときウィンドウプロシージャで呼び出す
	void OnChar(const unsigned char key);			// 文字が押されたときウィンドウプロシージャで呼び出す
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
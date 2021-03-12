#pragma once
#include "KeyboardEvent.h"
#include <queue>

class Keyboard
{
public:
	Keyboard();
	bool KeyIsPressed(const unsigned char keycode);
	bool KeyBufferIsEmpty();
	bool CharBufferIsEmpty();
	KeyboardEvent ReadKey();
	unsigned char ReadChar();
	void OnkeyPressed(const unsigned char key);
	void OnKeyReleased(const unsigned char key);
	void OnChar(const unsigned char key);
	void EnableAutoRepeatKeys();
	void DisableAutoRepeatKeys();
	void EnableAutoRepeatChars();
	void DisableAutoRepeatChars();
	bool IsKeysAutoRepeat();
	bool IsCharsAutoRepeat();

private:
	bool autoRepeatKeys = false;
	bool autoRepeateChars = false;
	bool keyStates[256];
	queue<KeyboardEvent> keyBuffer;
	queue<unsigned char> charBuffer;


};


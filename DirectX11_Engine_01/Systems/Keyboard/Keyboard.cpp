#include "../../stdafx.h"
#include "Keyboard.h"

Keyboard::Keyboard()
{
	for (int i = 0; i < 256; i++)
	{
		this->keyStates[i] = false;
	}
}

bool Keyboard::KeyIsPressed(const unsigned char keycode)
{
	return this->keyStates[keycode];
}

bool Keyboard::KeyBufferIsEmpty()
{
	return this->keyBuffer.empty();
}

bool Keyboard::CharBufferIsEmpty()
{
	return this->charBuffer.empty();
}

KeyboardEvent Keyboard::ReadKey()
{
	if (this->keyBuffer.empty())
		return KeyboardEvent();
	else
	{
		KeyboardEvent event = this->keyBuffer.front();
		this->keyBuffer.pop();
		return event;
	}
}

unsigned char Keyboard::ReadChar()
{
	if (this->charBuffer.empty())
		return 0u;
	else
	{
		unsigned char event = this->charBuffer.front();
		this->charBuffer.pop();
		return event;
	}
}

void Keyboard::OnkeyPressed(const unsigned char key)
{
	this->keyStates[key] = true;
	this->keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Press, key));
}

void Keyboard::OnKeyReleased(const unsigned char key)
{
	this->keyStates[key] = false;
	this->keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Release, key));
}

void Keyboard::OnChar(const unsigned char key)
{
	this->charBuffer.push(key);
}

void Keyboard::EnableAutoRepeatKeys()
{
	this->autoRepeatKeys = true;
}

void Keyboard::DisableAutoRepeatKeys()
{
	this->autoRepeatKeys = false;
}

void Keyboard::EnableAutoRepeatChars()
{
	this->autoRepeateChars = true;
}

void Keyboard::DisableAutoRepeatChars()
{
	this->autoRepeateChars = false;
}

bool Keyboard::IsKeysAutoRepeat()
{
	return this->autoRepeatKeys;
}

bool Keyboard::IsCharsAutoRepeat()
{
	return this->autoRepeateChars;
}

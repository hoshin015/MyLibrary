#pragma once

class KeyboardEvent
{
public:
	enum EventType
	{
		Press,		// ‰Ÿ‚µ‚½‚Æ‚«
		Release,	// —£‚µ‚½‚Æ‚«
		Invalid,	// –³Œø
	};

	KeyboardEvent();
	KeyboardEvent(const EventType type, const unsigned char key);
	bool IsPress() const;
	bool IsRelease() const;
	bool IsValid() const;
	unsigned char GetKeyCode() const;

private:
	EventType type;
	unsigned char key;
};
#pragma once
#include "IMessageManager.h"
#include "SettingTypes.h"
#include "../Utilities/Timer.h"

class Console;

class BaseRenderer : public IMessageManager
{
private:
	list<shared_ptr<ToastInfo>> _toasts;
	std::wstring WrapText(string utf8Text, float maxLineWidth, uint32_t &lineCount);
	virtual float MeasureString(std::wstring text) = 0;
	virtual bool ContainsCharacter(wchar_t character) = 0;

protected:
	shared_ptr<Console> _console;

	uint32_t _screenWidth = 0;
	uint32_t _screenHeight = 0;

	BaseRenderer(shared_ptr<Console> console, bool registerAsMessageManager);
	virtual ~BaseRenderer();

	void DisplayMessage(string title, string message);
	
	void DrawString(std::string message, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t opacity = 255);	
	virtual void DrawString(std::wstring message, int x, int y, uint8_t r = 255, uint8_t g = 255, uint8_t b = 255, uint8_t opacity = 255) = 0;
};

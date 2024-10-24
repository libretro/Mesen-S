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
};

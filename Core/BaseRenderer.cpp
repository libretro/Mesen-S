#include "stdafx.h"
#include <cmath>
#include "BaseRenderer.h"
#include "Console.h"
#include "EmuSettings.h"
#include "MessageManager.h"

BaseRenderer::BaseRenderer(shared_ptr<Console> console, bool registerAsMessageManager)
{
	_console = console;

	if(registerAsMessageManager) {
		//Only display messages on the master CPU's screen
		MessageManager::RegisterMessageManager(this);
	}
}

BaseRenderer::~BaseRenderer()
{
	MessageManager::UnregisterMessageManager(this);
}

void BaseRenderer::DisplayMessage(string title, string message)
{
	shared_ptr<ToastInfo> toast(new ToastInfo(title, message, 4000));
	_toasts.push_front(toast);
}

std::wstring BaseRenderer::WrapText(string utf8Text, float maxLineWidth, uint32_t &lineCount)
{
	using std::wstring;
	wstring text = utf8::utf8::decode(utf8Text);
	wstring wrappedText;
	list<wstring> words;
	wstring currentWord;
	for(size_t i = 0, len = text.length(); i < len; i++) {
		if(text[i] == L' ' || text[i] == L'\n') {
			if(currentWord.length() > 0) {
				words.push_back(currentWord);
				currentWord.clear();
			}
		} else {
			currentWord += text[i];
		}
	}
	if(currentWord.length() > 0) {
		words.push_back(currentWord);
	}

	lineCount = 1;
	float spaceWidth = MeasureString(L" ");

	float lineWidth = 0.0f;
	for(wstring word : words) {
		for(unsigned int i = 0; i < word.size(); i++) {
			if(!ContainsCharacter(word[i])) {
				word[i] = L'?';
			}
		}

		float wordWidth = MeasureString(word.c_str());

		if(lineWidth + wordWidth < maxLineWidth) {
			wrappedText += word + L" ";
			lineWidth += wordWidth + spaceWidth;
		} else {
			wrappedText += L"\n" + word + L" ";
			lineWidth = wordWidth + spaceWidth;
			lineCount++;
		}
	}

	return wrappedText;
}

void BaseRenderer::DrawString(std::string message, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t opacity)
{
	std::wstring textStr = utf8::utf8::decode(message);
	DrawString(textStr, x, y, r, g, b, opacity);
}

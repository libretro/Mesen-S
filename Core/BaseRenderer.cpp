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

void BaseRenderer::RemoveOldToasts()
{
	_toasts.remove_if([](shared_ptr<ToastInfo> toast) { return toast->IsToastExpired(); });
}

void BaseRenderer::DrawToasts()
{
	RemoveOldToasts();

	int counter = 0;
	int lastHeight = 5;
	for(shared_ptr<ToastInfo> toast : _toasts) {
		if(counter < 6) {
			DrawToast(toast, lastHeight);
		} else {
			break;
		}
		counter++;
	}
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

void BaseRenderer::DrawToast(shared_ptr<ToastInfo> toast, int &lastHeight)
{
	//Get opacity for fade in/out effect
	uint8_t opacity = (uint8_t)(toast->GetOpacity()*255);
	int textLeftMargin = 4;

	int lineHeight = 25;
	string text = "[" + toast->GetToastTitle() + "] " + toast->GetToastMessage();
	uint32_t lineCount = 0;
	std::wstring wrappedText = WrapText(text, (float)(_screenWidth - textLeftMargin * 2 - 20), lineCount);
	lastHeight += lineCount * lineHeight;
	DrawString(wrappedText, textLeftMargin, _screenHeight - lastHeight, opacity, opacity, opacity, opacity);
}

void BaseRenderer::DrawString(std::string message, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t opacity)
{
	std::wstring textStr = utf8::utf8::decode(message);
	DrawString(textStr, x, y, r, g, b, opacity);
}

bool BaseRenderer::IsMessageShown()
{
	return !_toasts.empty();
}	

#pragma once
#include "stdafx.h"

class Console;

class SaveStateManager
{
private:
	shared_ptr<Console> _console;
public:
	static constexpr uint32_t FileFormatVersion = 9;

	SaveStateManager(shared_ptr<Console> console);

	void GetSaveStateHeader(ostream & stream);

	void SaveState(ostream &stream);
	bool LoadState(istream &stream, bool hashCheckRequired = true);
};

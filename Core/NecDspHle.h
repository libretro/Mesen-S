#pragma once
#include "stdafx.h"
#include "BaseCoprocessor.h"
#include "dsp1emu.hpp"

class Console;
class MemoryManager;
class RamHandler;
enum class CoprocessorType;

class NecDspHle final : public BaseCoprocessor
{
private:
	Console* _console = nullptr;
	MemoryManager* _memoryManager = nullptr;
	Dsp1 _dsp1;
	unique_ptr<RamHandler> _ramHandler;
	CoprocessorType _type;

	double _frequency = 7600000;
	uint16_t _registerMask = 0;

	NecDspHle(CoprocessorType type, Console* console);

public:
	virtual ~NecDspHle();

	static NecDspHle* InitCoprocessor(CoprocessorType type, Console* console);

	void Reset() override;
	void Run() override;

	void LoadBattery() override;
	void SaveBattery() override;

	uint8_t Read(uint32_t addr) override;
	void Write(uint32_t addr, uint8_t value) override;

	uint8_t Peek(uint32_t addr) override;
	void PeekBlock(uint32_t addr, uint8_t * output) override;
	AddressInfo GetAbsoluteAddress(uint32_t address) override;

	void Serialize(Serializer &s) override;
};

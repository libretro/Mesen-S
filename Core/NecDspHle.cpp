#include "stdafx.h"
#include "NecDspHle.h"
#include "MemoryManager.h"
#include "MemoryMappings.h"
#include "Console.h"
#include "NotificationManager.h"
#include "BaseCartridge.h"
#include "CartTypes.h"
#include "MessageManager.h"
#include "EmuSettings.h"
#include "RamHandler.h"
#include "BatteryManager.h"
#include "FirmwareHelper.h"
#include "../Utilities/FolderUtilities.h"

NecDspHle::NecDspHle(CoprocessorType type, Console* console) : BaseCoprocessor(SnesMemoryType::Register)
{
	_console = console;
	_type = type;
	_memoryManager = console->GetMemoryManager().get();
	_memoryType = SnesMemoryType::Register;
	MemoryMappings *mm = _memoryManager->GetMemoryMappings();

	if(type == CoprocessorType::ST010 || type == CoprocessorType::ST011) {
		if(type == CoprocessorType::ST010) {
			_frequency = 11000000;
		} else {
			_frequency = 22000000;
		}
		_registerMask = 0x0001;
		mm->RegisterHandler(0x60, 0x60, 0x0000, 0x0FFF, this);
		mm->RegisterHandler(0xE0, 0xE0, 0x0000, 0x0FFF, this);
		mm->RegisterHandler(0x68, 0x6F, 0x0000, 0x0FFF, this);
		mm->RegisterHandler(0xE8, 0xEF, 0x0000, 0x0FFF, this);
	} else {
		_frequency = 7600000;
		if(console->GetCartridge()->GetCartFlags() & CartFlags::LoRom) {
			_registerMask = 0x4000;
			mm->RegisterHandler(0x30, 0x3F, 0x8000, 0xFFFF, this);
			mm->RegisterHandler(0xB0, 0xBF, 0x8000, 0xFFFF, this);

			//For Super Bases Loaded 2
			mm->RegisterHandler(0x60, 0x6F, 0x0000, 0x7FFF, this);
			mm->RegisterHandler(0xE0, 0xEF, 0x0000, 0x7FFF, this);
		} else if(console->GetCartridge()->GetCartFlags() & CartFlags::HiRom) {
			_registerMask = 0x1000;
			mm->RegisterHandler(0x00, 0x1F, 0x6000, 0x7FFF, this);
			mm->RegisterHandler(0x80, 0x9F, 0x6000, 0x7FFF, this);
		}
	}
}

NecDspHle::~NecDspHle()
{
}

NecDspHle* NecDspHle::InitCoprocessor(CoprocessorType type, Console *console)
{
	bool supported = false;
	switch(type) {
		// Probably need more work to support properly dsp1 vs dsp1b
		case CoprocessorType::DSP1:
		case CoprocessorType::DSP1B: supported = true; break;
		default: supported = false;
	}

	if(!supported) {
		return nullptr;
	}

	return new NecDspHle(type, console);
}

void NecDspHle::Reset()
{
	if (_type == CoprocessorType::DSP1 || _type == CoprocessorType::DSP1B) {
		//printf("[DSP] Reset\n");
		_dsp1.reset();
	}
}

void NecDspHle::Run()
{
}

void NecDspHle::LoadBattery()
{
}

void NecDspHle::SaveBattery()
{
}

uint8_t NecDspHle::Read(uint32_t addr)
{
	if (_type == CoprocessorType::DSP1 || _type == CoprocessorType::DSP1B) {
		uint8_t r;
		if(addr & _registerMask) {
			r = _dsp1.getSr();
		} else {
			r = _dsp1.getDr();
		}
		//printf("[DSP] Read (%x) => %d\n", addr, r);
		return r;
	}
	return 0;
}

void NecDspHle::Write(uint32_t addr, uint8_t value)
{
	if (_type == CoprocessorType::DSP1 || _type == CoprocessorType::DSP1B) {
		if(addr & _registerMask) {
			return;
		} else {
			//printf("[DSP] Write (%x) <= %d\n", addr, value);
			return _dsp1.setDr(value);
		}
	}
}

uint8_t NecDspHle::Peek(uint32_t addr)
{
	//Avoid side effects for now
	return 0;
}

void NecDspHle::PeekBlock(uint32_t addr, uint8_t *output)
{
	memset(output, 0, 0x1000);
}

AddressInfo NecDspHle::GetAbsoluteAddress(uint32_t address)
{
	return { -1, SnesMemoryType::Register };
}

void NecDspHle::Serialize(Serializer &s)
{
	if (_type == CoprocessorType::DSP1 || _type == CoprocessorType::DSP1B) {
		_dsp1.Serialize(s);
	}

#if 0
	s.Stream(
		_state.A, _state.B, _state.DP, _state.DR, _state.K, _state.L, _state.M, _state.N, _state.PC,
		_state.RP, _state.SerialIn, _state.SerialOut, _state.SP, _state.SR, _state.TR, _state.TRB, 
		_state.FlagsA.Carry, _state.FlagsA.Overflow0, _state.FlagsA.Overflow1, _state.FlagsA.Sign0, _state.FlagsA.Sign1, _state.FlagsA.Zero,
		_state.FlagsB.Carry, _state.FlagsB.Overflow0, _state.FlagsB.Overflow1, _state.FlagsB.Sign0, _state.FlagsB.Sign1, _state.FlagsB.Zero
	);

	s.Stream(_opCode, _cycleCount, _inRqmLoop);
	s.StreamArray<uint16_t>(_ram, _ramSize);
	s.StreamArray<uint16_t>(_stack, _stackSize);
#endif
}


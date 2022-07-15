#include "stdafx.h"
#include "NecDsp.h"
#include "NecDspDebugger.h"
#include "DisassemblyInfo.h"
#include "Disassembler.h"
#include "CallstackManager.h"
#include "BreakpointManager.h"
#include "BaseCartridge.h"
#include "MemoryManager.h"
#include "Debugger.h"
#include "Console.h"
#include "MemoryAccessCounter.h"
#include "ExpressionEvaluator.h"
#include "EmuSettings.h"

NecDspDebugger::NecDspDebugger(Debugger* debugger)
{
	_debugger = debugger;
	_disassembler = debugger->GetDisassembler().get();
	_dsp = debugger->GetConsole()->GetCartridge()->GetDsp();
	_settings = debugger->GetConsole()->GetSettings().get();

	_breakpointManager.reset(new BreakpointManager(debugger, CpuType::NecDsp));
	_step.reset(new StepRequest());
}

void NecDspDebugger::Reset()
{
}

void NecDspDebugger::ProcessRead(uint16_t addr, uint8_t value, MemoryOperationType type)
{
	AddressInfo addressInfo = { (int32_t)addr, type == MemoryOperationType::ExecOpCode ? SnesMemoryType::DspProgramRom : SnesMemoryType::DspDataRom };
	MemoryOperationInfo operation { (uint32_t)addr, value, type };

	if(type == MemoryOperationType::ExecOpCode) {
		if(_settings->CheckDebuggerFlag(DebuggerFlags::NecDspDebuggerEnabled)) {
			_disassembler->BuildCache(addressInfo, 0, CpuType::NecDsp);
		}

		_prevProgramCounter = addr;

		if(_step->StepCount > 0) {
			_step->StepCount--;
		}
	}

	_debugger->ProcessBreakConditions(_step->StepCount == 0, GetBreakpointManager(), operation, addressInfo);
}

void NecDspDebugger::ProcessWrite(uint16_t addr, uint8_t value, MemoryOperationType type)
{
	AddressInfo addressInfo { addr, SnesMemoryType::DspDataRam }; //Writes never affect the DSP ROM
	MemoryOperationInfo operation { addr, value, type };
	_debugger->ProcessBreakConditions(false, GetBreakpointManager(), operation, addressInfo);
}

void NecDspDebugger::Run()
{
	_step.reset(new StepRequest());
}

void NecDspDebugger::Step(int32_t stepCount, StepType type)
{
	StepRequest step;

	switch(type) {
		case StepType::Step: step.StepCount = stepCount; break;
		
		case StepType::StepOut:
		case StepType::StepOver:
			step.StepCount = 1;
			break;

		case StepType::SpecificScanline:
		case StepType::PpuStep:
			break;
	}

	_step.reset(new StepRequest(step));
}

shared_ptr<CallstackManager> NecDspDebugger::GetCallstackManager()
{
	return nullptr;
}

BreakpointManager* NecDspDebugger::GetBreakpointManager()
{
	return _breakpointManager.get();
}

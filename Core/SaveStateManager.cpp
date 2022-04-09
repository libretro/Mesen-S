#include "stdafx.h"
#include "../Utilities/FolderUtilities.h"
#include "../Utilities/ZipWriter.h"
#include "../Utilities/ZipReader.h"
#include "../Utilities/PNGHelper.h"
#include "SaveStateManager.h"
#include "MessageManager.h"
#include "Console.h"
#include "EmuSettings.h"
#include "VideoDecoder.h"
#include "BaseCartridge.h"
#include "MovieManager.h"
#include "EventType.h"
#include "Debugger.h"
#include "Ppu.h"
#include "DefaultVideoFilter.h"

SaveStateManager::SaveStateManager(shared_ptr<Console> console)
{
	_console = console;
}

void SaveStateManager::GetSaveStateHeader(ostream &stream)
{
	uint32_t emuVersion = _console->GetSettings()->GetVersion();
	uint32_t formatVersion = SaveStateManager::FileFormatVersion;
	stream.write("MSS", 3);
	stream.write((char*)&emuVersion, sizeof(emuVersion));
	stream.write((char*)&formatVersion, sizeof(uint32_t));

	// Implementation of sha1hash is really sucky (lots of string/array copies)
	// The sha1 isn't even checked at load time...
	//string sha1Hash = _console->GetCartridge()->GetSha1Hash();
	string sha1Hash(40, 'X'); // Fill a dummy pattern to keep save compatibilities
	stream.write(sha1Hash.c_str(), sha1Hash.size());

	bool isGameboyMode = _console->GetSettings()->CheckFlag(EmulationFlags::GameboyMode);
	stream.write((char*)&isGameboyMode, sizeof(bool));

	RomInfo romInfo = _console->GetCartridge()->GetRomInfo();
	string romName = FolderUtilities::GetFilename(romInfo.RomFile.GetFileName(), true);
	uint32_t nameLength = (uint32_t)romName.size();
	stream.write((char*)&nameLength, sizeof(uint32_t));
	stream.write(romName.c_str(), romName.size());
}

void SaveStateManager::SaveState(ostream &stream)
{
	GetSaveStateHeader(stream);
	_console->Serialize(stream);
}

bool SaveStateManager::LoadState(istream &stream, bool hashCheckRequired)
{
	char header[3];
	stream.read(header, 3);
	if(memcmp(header, "MSS", 3) == 0) {
		uint32_t emuVersion, fileFormatVersion;

		stream.read((char*)&emuVersion, sizeof(emuVersion));
		if(emuVersion > _console->GetSettings()->GetVersion()) {
			return false;
		}

		stream.read((char*)&fileFormatVersion, sizeof(fileFormatVersion));
		if(fileFormatVersion <= 5)
			return false;
		{
			char hash[41] = {};
			stream.read(hash, 40);

			if(fileFormatVersion >= 8)
			{
				bool isGameboyMode = false;
				stream.read((char*)&isGameboyMode, sizeof(bool));
				if(isGameboyMode != _console->GetSettings()->CheckFlag(EmulationFlags::GameboyMode))
					return false;
			} 
			
			if(fileFormatVersion >= 7) {
			}

			uint32_t nameLength = 0;
			stream.read((char*)&nameLength, sizeof(uint32_t));
			
			vector<char> nameBuffer(nameLength);
			stream.read(nameBuffer.data(), nameBuffer.size());
			string romName(nameBuffer.data(), nameLength);
			
			shared_ptr<BaseCartridge> cartridge = _console->GetCartridge();
			if(!cartridge /*|| cartridge->GetSha1Hash() != string(hash)*/) {
				//Game isn't loaded, or CRC doesn't match
				//TODO: Try to find and load the game
				return false;
			}
		}

		//Stop any movie that might have been playing/recording if a state is loaded
		//(Note: Loading a state is disabled in the UI while a movie is playing/recording)
		_console->GetMovieManager()->Stop();

		bool is_compressed = fileFormatVersion <= 8;
		_console->Deserialize(stream, fileFormatVersion, is_compressed);

		return true;
	}
	return false;
}

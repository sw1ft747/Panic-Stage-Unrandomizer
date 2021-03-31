// C++
// Patch Data Header

#ifndef PATCH_DATA_H
#define PATCH_DATA_H

namespace PatchData
{
	namespace Signatures
	{
		unsigned char *StartPanicEvent = (unsigned char *)"\xD9\x05\xCC\xCC\xCC\xCC\x83\xC4\x04\xD9\x5C\x24\x04\x89\xBE\xCC\xCC\xCC\xCC\xD9\xE8";
		const char *StartPanicEventMask = "xx????xxxxxxxxx????xx";

		unsigned char *SpawnMegaMob = (unsigned char *)"\xD9\x05\xCC\xCC\xCC\xCC\x56\x57\x83\xEC\x08\xD9\x5C\x24\x04\x8B\xF9\xD9\x05";
		const char *SpawnMegaMobMask = "xx????xxxxxxxxxxxxx";
	}

	unsigned char StartPanicEvent_PatchedBytes[6] =
	{
		0xD9, 0xE8, // FLD1
		0x90,		// NOP
		0x90,		// NOP
		0x90,		// NOP
		0x90		// NOP
	};
}

#endif
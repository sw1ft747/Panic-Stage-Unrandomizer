// C++
// Signature Scanner Header

#ifndef SIGNATURE_SCANNER_H
#define SIGNATURE_SCANNER_H

#include <Windows.h>
#include <psapi.h>

void *FindPattern(const wchar_t *wcModuleName, unsigned char *szSignature, const char *szMask)
{
	MODULEINFO mInfo = {0};
	HMODULE hModule = GetModuleHandle(wcModuleName);

	if (hModule == nullptr)
		return nullptr;

	GetModuleInformation(GetCurrentProcess(), hModule, &mInfo, sizeof(MODULEINFO));

	size_t i, nMaskLength = strlen(szMask);

	BYTE *pModuleBase = (BYTE *)mInfo.lpBaseOfDll;
	BYTE *pModuleEnd = pModuleBase + mInfo.SizeOfImage - (DWORD)nMaskLength;

	while (pModuleBase < pModuleEnd)
	{
		for (i = 0; i < nMaskLength; i++)
		{
			if (szMask[i] != '?' && szSignature[i] != pModuleBase[i])
				break;
		}

		if (i == nMaskLength)
			return (void *)pModuleBase;

		pModuleBase++;
	}

	return nullptr;
}

#endif
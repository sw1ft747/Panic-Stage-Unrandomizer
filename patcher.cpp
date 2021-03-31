// C++
// Patcher

#include "patcher.h"

CPatcher::CPatcher(const void *pPatchAddress, const void *pPatchedBytes, const int nPatchLength) : m_PatchAddress(nullptr), m_nPatchLength(0)
{
	m_szOriginalBytes = (BYTE *)calloc(nPatchLength + 1, sizeof(BYTE));
	m_szPatchedBytes = (BYTE *)calloc(nPatchLength + 1, sizeof(BYTE));

	if (m_szOriginalBytes == nullptr || m_szPatchedBytes == nullptr)
	{
		printf("CPatcher::constructor(): Failed to allocate memory!");
		return;
	}

	m_PatchAddress = (BYTE *)pPatchAddress;
	m_nPatchLength = nPatchLength;

	memcpy(m_szOriginalBytes, pPatchAddress, nPatchLength);
	memcpy(m_szPatchedBytes, pPatchedBytes, nPatchLength);
}

CPatcher::~CPatcher()
{
	delete m_szOriginalBytes;
	delete m_szPatchedBytes;
}

bool CPatcher::Patch()
{
	if (m_szOriginalBytes == nullptr || m_szPatchedBytes == nullptr)
		return false;

	DWORD dwProtection, dwTemp;
	VirtualProtect(m_PatchAddress, m_nPatchLength, PAGE_EXECUTE_READWRITE, &dwProtection);

	memcpy(m_PatchAddress, m_szPatchedBytes, m_nPatchLength);

	VirtualProtect(m_PatchAddress, m_nPatchLength, dwProtection, &dwTemp);

	return true;
}

bool CPatcher::Unpatch()
{
	if (m_szOriginalBytes == nullptr || m_szPatchedBytes == nullptr)
		return false;

	DWORD dwProtection, dwTemp;
	VirtualProtect(m_PatchAddress, m_nPatchLength, PAGE_EXECUTE_READWRITE, &dwProtection);

	memcpy(m_PatchAddress, m_szOriginalBytes, m_nPatchLength);

	VirtualProtect(m_PatchAddress, m_nPatchLength, dwProtection, &dwTemp);

	return true;
}
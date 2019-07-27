#pragma once
#include <Windows.h>

class CPEInfo
{
public:
	CPEInfo(void);
	~CPEInfo(void);
protected:
	PBYTE m_pImageBase;
protected:
	PIMAGE_DOS_HEADER m_pDosHeader;
	
	//PE Head
	PIMAGE_NT_HEADERS m_pNTHeaders;
	PIMAGE_FILE_HEADER m_pFileHeader;
	PIMAGE_OPTIONAL_HEADER32 m_pOptionalHeader; 
	
	PIMAGE_SECTION_HEADER m_pSectionHeader;
protected:
	
	DWORD m_EntryPointVA;
	//Code Section Header Index
	int m_SHCodeIndex;
	
	WORD m_numberOfSections;

	PBYTE m_pCodeData;
	
	DWORD m_CodeSize;
	DWORD m_BaseAddress;
protected:
//***********************************************************
//Read Dos Header
//***********************************************************
    BOOL ReadDosHeader();
//***********************************************************
//Read NT Headers
//***********************************************************	
	BOOL ReadNTHeaders();
//***********************************************************
//Read Code Data
//***********************************************************	
	BOOL ReadCodeData();
//***********************************************************
//Read Section Table
//***********************************************************	
	BOOL ReadSectionTable();

public:
	BOOL Parse(PBYTE pImageBase);
	
	inline DWORD GetBaseAddress(){return m_BaseAddress;}
	inline DWORD GetCodeDataSize(){return m_CodeSize;}
	inline PBYTE GetCodeData(){return m_pCodeData;}
	inline BYTE  GetMajorLinkerVersion()const{return m_pOptionalHeader->MajorLinkerVersion;}
};
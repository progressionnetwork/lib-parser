#pragma once
#include <stdio.h>
#include <windows.h>
#include <io.h>
#include <vector>
using std::vector;


#define IMAGE_FLIB_START_SIZE             8
#define IMAGE_FLIB_START                  "!<flib>\n"

typedef struct _FuncHeader
{
	DWORD NameOff;
	DWORD NameSize;
	DWORD DataOff;
	DWORD DataSize;
}FuncHeader,*PFuncHeader;
typedef struct _FlibFuncHeader
{
	DWORD NameOff;
	DWORD NameSize;
	DWORD DataOff;
	DWORD DataSize;
}FlibFuncHeader,*PFlibFuncHeader;
typedef vector<FuncHeader> FuncHeaderTable;

class CLibParser
{
public:
	CLibParser(void);
	~CLibParser(void);
protected:
	PBYTE m_pLibImage;
	long  m_fsize;
	FILE* m_pFlibFile;
	FILE* m_pNameFile;
	FILE* m_pDataFile;
	CHAR  m_FlibFileName[MAX_PATH];
	CHAR  m_NameFileName[MAX_PATH];
	CHAR  m_DataFileName[MAX_PATH];
	FuncHeaderTable m_FuncTable;
protected:
	BOOL LoadLib(PCSTR szLib);
	PBYTE GetFirstObjSection();
	BOOL InitOutPutFile(PCSTR szLib);
	BOOL ParseObjs(PBYTE pObjSect, PCSTR szLib);
	void LinkFile();

	void ModifySuffix(PCHAR filename,PCHAR pSuffix);
	BOOL fopen_S(FILE ** _File, PCSTR _Filename,PCSTR _Mode);
	BOOL bImportlibraryFormat(PBYTE pSect);
public:
	BOOL Parse(PCSTR szLib);
};

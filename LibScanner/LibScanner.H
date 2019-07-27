#ifndef	_CLIBSCANNER_H_
#define	_CLIBSCANNER_H_

#include <iostream>
#include <tchar.h>
#include <windows.h>
#include <assert.h>
#include <io.h>

#define IMAGE_FLB_START_SIZE             8
#define IMAGE_FLB_START                  "!<flib>\n"

typedef const BYTE* PCBYTE;

typedef struct _FuncHeader//flb
{
	DWORD NameOff;
	DWORD DataOff;
	DWORD DataSize;
}FuncHeader,*PFuncHeader;

class CLibScanner
{
 
public:
    CLibScanner();
    ~CLibScanner();
protected:
	PBYTE m_pImage;
	PFuncHeader m_pFuncHeader;
protected:
	BOOL CheckFunc(PFuncHeader pFuncHeader,PCBYTE phead);
public:
    PCSTR CheckIfLibFunc(PCBYTE phead);
    BOOL  ScanLib(LPCSTR szLib);
};


BOOL InitLibScanner(BYTE MajorLinkerVersion);
PCHAR CheckIfLibFunc(PBYTE pFuncData);


#endif	// _CLIBSCANNER_H_

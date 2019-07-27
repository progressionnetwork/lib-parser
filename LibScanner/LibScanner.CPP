//leeeryan	leeeryan@gmail.com

#include "LibScanner.h"

CLibScanner:: CLibScanner():
m_pImage(NULL),m_pFuncHeader(NULL)
{

}

CLibScanner::~CLibScanner()
{	
	if(m_pImage)free(m_pImage);
}


BOOL  CLibScanner::ScanLib(LPCSTR szLib)
{
	FILE* pFile;
	fopen_s(&pFile,szLib,"rb");
	if (!pFile)
	{
		CHAR ErrorStr[MAX_PATH];
		sprintf_s(ErrorStr,MAX_PATH,"Can't Open %s",szLib);
		MessageBox(NULL,ErrorStr,"Error",MB_ICONWARNING);
		return FALSE;
	}
	int len=_filelength(_fileno(pFile));
	m_pImage=(PBYTE)malloc(len);
	fread_s(m_pImage,len,len,1,pFile);
	fclose(pFile);

	if (memcmp(m_pImage,IMAGE_FLB_START,IMAGE_FLB_START_SIZE)!=0)
	{
		return FALSE;
	}
	
	m_pFuncHeader=(PFuncHeader)(m_pImage+IMAGE_FLB_START_SIZE);
	
	fclose(pFile);
    return TRUE;
}


PCSTR CLibScanner::CheckIfLibFunc(PCBYTE pFuncDat)
{
	PFuncHeader pFuncHeader=m_pFuncHeader;
	PCSTR pNam=NULL;

	do 
	{
		if(CheckFunc(pFuncHeader,pFuncDat))
		{	
			pNam=(PCSTR)(m_pImage+pFuncHeader->NameOff);
			return pNam;
		}
		pFuncHeader++;
	} while (pFuncHeader->DataSize);

    return NULL;
}
BOOL CLibScanner::CheckFunc(PFuncHeader pFuncHeader,PCBYTE pFuncDat)
{
	PBYTE pImpFuncDat=m_pImage+pFuncHeader->DataOff;

	for (DWORD i=0;i<pFuncHeader->DataSize;i++)
	{
		while(*(PDWORD)(pImpFuncDat+i)==0)
		{	
			i+=4;
		}
		if (pImpFuncDat[i]!=pFuncDat[i])
		{
			return FALSE;
		}
	}
	return TRUE;
}
/*=========================================CLibScanner END=============================================*/

CLibScanner LibScannerRL,LibScannerDB;

BOOL InitLibScanner(BYTE MajorLinkerVersion)
{
	char* pReleaseLibName;
	char* pDebugLibName;
	switch(MajorLinkerVersion)
	{
	case 6:
		pReleaseLibName = "..\\Libs\\VC6Lib\\libc.flb";
		pDebugLibName = "..\\Libs\\VC6Lib\\libcd.flb";
		break;
	case 7:
		pReleaseLibName = "..\\Libs\\VC2003Lib\\libc.flb";
		pDebugLibName = "..\\Libs\\VC2003Lib\\libcd.flb";
		break;
	case 8:
		pReleaseLibName = "..\\Libs\\VC2005Lib\\libcmt.flb"; //VC2005
		pDebugLibName = "..\\Libs\\VC2005Lib\\libcmtd.flb";
		break;
	case 9: 
		pReleaseLibName = "..\\Libs\\VC2008Lib\\libcmt.flb"; //VC2008
		pDebugLibName = "..\\Libs\\VC2008Lib\\libcmtd.flb";
		break;
	}

	if(!LibScannerDB.ScanLib(pDebugLibName))
	{
		MessageBox(NULL,"ScanLib Failed!","Error",MB_ICONWARNING);
		return FALSE;
	}
	if(!LibScannerRL.ScanLib(pReleaseLibName))
	{
		MessageBox(NULL,"ScanLib Failed!","Error",MB_ICONWARNING);
		return FALSE;
	}
	return TRUE;
}


PCHAR CheckIfLibFunc(PBYTE pFuncData)
{
	PCHAR pFuncName;
	pFuncName = (PCHAR)LibScannerDB.CheckIfLibFunc(pFuncData);
	if(!pFuncName)
	{
		pFuncName = (PCHAR)LibScannerRL.CheckIfLibFunc(pFuncData);
	}
	return pFuncName;
}
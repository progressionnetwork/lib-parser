// LibScanner.cpp : leeeryan	leeeryan@gmail.com

#include <stdio.h>
#include <tchar.h>
#include <io.h>
#include "PEInfo.h"
#include "LibScanner.h"
#include "PVdasm\\Disasm.h"

void ModifySuffix(PCHAR filename,PCHAR pSuffix)
{
	PCHAR pDest=strrchr(filename,'.');
	do 
	{
		*pDest++=*pSuffix++;
	} while (*pDest&&*pSuffix);
	*pDest=0;
}

FILE* pLogFile=NULL;

int _tmain(int argc, _TCHAR* argv[])
{
	FILE* pFile=NULL;
	PBYTE PEImage=NULL;
	PBYTE pCodeData=NULL;
	DWORD baseAddress=0;
	unsigned int CodeDatSize=0;
	BYTE MajorLinkerVersion=0;
	PCHAR PEFileNam = "..\\test.exe";
	CHAR  LogFileNam[MAX_PATH];

#pragma region a0
	fopen_s(&pFile,PEFileNam,"rb");
	if(!pFile)
	{
		printf("Error:Can't open %s",PEFileNam);
		return 0;
	}
	const unsigned int fileLen=_filelength(_fileno(pFile));
	PEImage=new BYTE[fileLen];
	fread_s(PEImage,fileLen,fileLen,1,pFile);
	fclose(pFile);
#pragma endregion 
	
#pragma region a1
	CPEInfo PEInfo;
	if(!PEInfo.Parse(PEImage))
		return 0;
	pCodeData=PEInfo.GetCodeData();
	CodeDatSize=PEInfo.GetCodeDataSize();
	MajorLinkerVersion=PEInfo.GetMajorLinkerVersion();
	baseAddress=PEInfo.GetBaseAddress();
#pragma endregion 

	if(!InitLibScanner(MajorLinkerVersion))
		return 0;
	
#pragma region a2
	strcpy_s(LogFileNam,MAX_PATH,PEFileNam);
	ModifySuffix(LogFileNam,".log");
	fopen_s(&pLogFile,LogFileNam,"wb");
	if(!pFile)
	{
		printf("Error:Can't open %s\n",PEFileNam);
		return 0;
	}
	PVDasm(pCodeData,CodeDatSize,baseAddress,pLogFile);
	fclose(pLogFile);
#pragma endregion 
	
	printf("PEFile %s Analysis Succeed!\n",PEFileNam);
	delete[] PEImage;
	return 0;
}


extern void ShowDecoded(DISASSEMBLY* pDisasm,FILE* pfile);

//call(0xe8)
void CallHandle(PBYTE pCallData,DISASSEMBLY* pDisasm)
{
	PCHAR pLibFuncNam=NULL;
	while(*pCallData==0xe9)
	{
		int jumpOffset=*(int*)(pCallData+1);
		pCallData=pCallData+jumpOffset+5;
	}
	
	pLibFuncNam=CheckIfLibFunc(pCallData);
	if(pLibFuncNam)
	{
		ShowDecoded(pDisasm,pLogFile);
		fprintf_s(pLogFile,"Call LibFunc:%s\n",pLibFuncNam);
	}
}
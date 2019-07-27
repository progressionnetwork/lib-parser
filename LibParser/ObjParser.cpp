#include "ObjParser.h"

CObjParser::CObjParser(void):
m_pObjImage(NULL),m_pFileHeader(NULL),m_pSectionHeader(NULL),m_pRelocation(NULL),
m_pStrings(NULL),m_pNamFile(NULL),m_pDatFile(NULL),m_pFuncTable(NULL)
{
}

CObjParser::~CObjParser(void)
{
	//if(m_pObjImage) delete m_pObjImage;
}


BOOL CObjParser::Parse(PBYTE objImage,FILE* pNamFile,FILE* pDatFile,FuncHeaderTable* funcTable, CHAR* m_NameFileName)
{
	m_pObjImage=objImage;
	m_pNamFile=pNamFile;
	m_pDatFile=pDatFile;
	m_pFuncTable=funcTable;
	
	m_pFileHeader=(PIMAGE_FILE_HEADER)objImage;
	m_pSectionHeader=(PIMAGE_SECTION_HEADER)(objImage+sizeof(IMAGE_FILE_HEADER));
	m_pSymbol=(PIMAGE_SYMBOL)(objImage+m_pFileHeader->PointerToSymbolTable);
	m_pStrings=(PCHAR)(m_pSymbol+m_pFileHeader->NumberOfSymbols);

		
	PIMAGE_SYMBOL pSymbol;

	FuncHeader funcHeader;

	for (DWORD i=0;i<m_pFileHeader->NumberOfSymbols;i++)
	{
		pSymbol=m_pSymbol+i;

		if(ISFCN(pSymbol->Type)&&pSymbol->SectionNumber>0
			&&pSymbol->StorageClass==IMAGE_SYM_CLASS_EXTERNAL)
		{
			memset(&funcHeader,0,sizeof(funcHeader));

			GetNameofSymb(pSymbol,funcHeader, m_NameFileName);
			
			GetDataofSymb(pSymbol,funcHeader);	

			m_pFuncTable->push_back(funcHeader);
		}

		i+=pSymbol->NumberOfAuxSymbols;
	}

	return TRUE;
}


void CObjParser::GetNameofSymb(PIMAGE_SYMBOL pSymbol,FuncHeader& funcHeader, CHAR* m_NameFileName)
{
	PCHAR pName=NULL;
	CHAR shortNam[9]={0};
	std::string import_lib;

	if (pSymbol->N.Name.Short)
	{
		
		memcpy_s(shortNam,9,pSymbol->N.ShortName,8);

		pName=shortNam;
	} 
	else
	{
		pName= m_pStrings+pSymbol->N.Name.Long;
	}

	import_lib = pName;
	printf("%s\n", import_lib.c_str());

	if (m_pFuncTable->size()==0)
	{
		funcHeader.NameOff=0;
	} 
	else
	{
		FuncHeader& funcHeadPrev=m_pFuncTable->at(m_pFuncTable->size()-1);
		funcHeader.NameOff=funcHeadPrev.NameOff+funcHeadPrev.NameSize;
	}

	funcHeader.NameSize=strlen(pName)+1;
	
	fwrite(pName,funcHeader.NameSize,1,m_pNamFile);
	fflush(m_pNamFile);
}


void CObjParser::GetDataofSymb(PIMAGE_SYMBOL pSymbol,FuncHeader& funcHeader)
{
	PIMAGE_SECTION_HEADER pISH = m_pSectionHeader+(pSymbol->SectionNumber-1);
	if(!pISH)
	{
		MessageBoxA(NULL,"Get SectionHeader Error!","Error",MB_ICONWARNING);
		return;
	}

	if (m_pFuncTable->size()==0)
	{
		funcHeader.DataOff=0;
	} 
	else
	{
		FuncHeader& funcHeadPrev=m_pFuncTable->at(m_pFuncTable->size()-1);
		funcHeader.DataOff=funcHeadPrev.DataOff+funcHeadPrev.DataSize;
	}	

	funcHeader.DataSize=pISH->SizeOfRawData-pSymbol->Value;

	MarkRelocatePos(pISH);

	PBYTE funData=m_pObjImage+pISH->PointerToRawData+pSymbol->Value;
	
	fwrite(funData,funcHeader.DataSize,1,m_pDatFile);
	fflush(m_pDatFile);
}


void CObjParser::MarkRelocatePos(PIMAGE_SECTION_HEADER pISH)
{
	
	DWORD pReloMark=0;
	DWORD modifyOff=0;
	
	PIMAGE_RELOCATION pIR = (PIMAGE_RELOCATION)(m_pObjImage + pISH->PointerToRelocations);

	DWORD RefCount = pISH->NumberOfRelocations;
	for(DWORD i =0;i<RefCount;i++)
	{
		
		modifyOff=pISH->PointerToRawData+pIR[i].VirtualAddress;
		
		memcpy_s(m_pObjImage+modifyOff,4,&pReloMark,4);		
	}
}	
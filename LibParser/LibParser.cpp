// leeeryan	leeeryan@gmail.com


#include "LibParser.h"
#include "ObjParser.h"

CLibParser::CLibParser(void):
m_pLibImage(NULL),
m_pFlibFile(NULL),m_pNameFile(NULL),m_pDataFile(NULL)
{
}

CLibParser::~CLibParser(void)
{
	if(m_pLibImage)delete[] m_pLibImage;
}

BOOL CLibParser::Parse(PCSTR szLib)
{
	if(!LoadLib(szLib))
		return FALSE;

	PBYTE pObjSect=GetFirstObjSection();
	if(!pObjSect)
	{
		MessageBoxA(NULL,"This Lib is error!","Error",MB_ICONWARNING);
		return FALSE;
	}

	if(!InitOutPutFile(szLib))
		return FALSE;

	if(!ParseObjs(pObjSect, szLib))
		return FALSE;
	
	LinkFile();

	return TRUE;
}

BOOL CLibParser::ParseObjs(PBYTE pObjSect, PCSTR szLib)
{
	do 
	{
		PIMAGE_ARCHIVE_MEMBER_HEADER pAME=(PIMAGE_ARCHIVE_MEMBER_HEADER)pObjSect;
		pObjSect+=sizeof(IMAGE_ARCHIVE_MEMBER_HEADER);

		
		if(bImportlibraryFormat(pObjSect))
		{
			MessageBoxA(NULL,"This is not a Archive Format File,it's a Import Format File!",
				"WARNING",MB_ICONWARNING);
			return FALSE;
		}

		CObjParser objParser;
		objParser.Parse(pObjSect,m_pNameFile,m_pDataFile,&m_FuncTable, (char*)szLib);
	
		pObjSect += atol((LPSTR)pAME->Size) ;
		
		if(*pObjSect=='\n') 
			pObjSect++;

	} while (pObjSect<m_pLibImage+m_fsize);

	return TRUE;
}


void CLibParser::LinkFile()
{
	
	DWORD sizeofFuncHeader=(m_FuncTable.size()+1)*sizeof(FlibFuncHeader);
	DWORD sizeofNamSection=_filelength(_fileno(m_pNameFile));
	DWORD sizeofDatSection=_filelength(_fileno(m_pDataFile));
	
	DWORD baseNameOff=IMAGE_FLIB_START_SIZE+sizeofFuncHeader;
	DWORD baseDataOff=baseNameOff+sizeofNamSection;
	
	fclose(m_pNameFile);
	fopen_S(&m_pNameFile,m_NameFileName,"rb");
	PBYTE pNamSection=(PBYTE)malloc(sizeofNamSection+1);
	fread(pNamSection,sizeofNamSection,1,m_pNameFile);

	for (int i = 0; i < sizeofNamSection; i++)
	{
		if (pNamSection[i] == 0)
			pNamSection[i] = '\n';
	}

	fclose(m_pDataFile);
	fopen_S(&m_pDataFile,m_DataFileName,"rb");
	PBYTE pDatSection=(PBYTE)malloc(sizeofDatSection);
	fread(pDatSection,sizeofDatSection,1,m_pDataFile);

	FlibFuncHeader funcHeader;
	
	fwrite(&IMAGE_FLIB_START,IMAGE_FLIB_START_SIZE,1,m_pFlibFile);
	fflush(m_pFlibFile);
	
	FuncHeaderTable::const_iterator it=m_FuncTable.begin();
	for (;it!=m_FuncTable.end();++it)
	{
		memset(&funcHeader,0,sizeof(funcHeader));

		funcHeader.NameOff=(*it).NameOff+baseNameOff;
		funcHeader.DataOff=(*it).DataOff+baseDataOff;
		funcHeader.DataSize=(*it).DataSize;
		
		fwrite(&funcHeader,sizeof(funcHeader),1,m_pFlibFile);
		fflush(m_pFlibFile);
	}
	
	memset(&funcHeader,0,sizeof(funcHeader));
	fwrite(&funcHeader,sizeof(funcHeader),1,m_pFlibFile);
	fflush(m_pFlibFile);

	fwrite(pNamSection,sizeofNamSection,1,m_pFlibFile);
	fflush(m_pFlibFile);
	
	fwrite(pDatSection,sizeofDatSection,1,m_pFlibFile);
	fflush(m_pFlibFile);
	
	free(pNamSection);
	free(pDatSection);

	
	fclose(m_pFlibFile);
	fclose(m_pNameFile);
	fclose(m_pDataFile);
	
	//remove(m_FlibFileName);
	//remove(m_NameFileName);
	//remove(m_DataFileName);
}

BOOL CLibParser::LoadLib(PCSTR szLib)
{
	FILE * pFile ;
	if(!fopen_S(&pFile,szLib,"rb"))
		return FALSE;
	m_fsize = _filelength(_fileno(pFile));

	m_pLibImage = new BYTE[m_fsize];
	if (m_pLibImage == NULL)
	{
		MessageBoxA(NULL,"Can't Allocate For Lib!","Error",MB_ICONWARNING);
		fclose(pFile);
		return FALSE;
	}
	fread(m_pLibImage,m_fsize,1,pFile);

	if(memcmp(m_pLibImage,IMAGE_ARCHIVE_START,IMAGE_ARCHIVE_START_SIZE)!=0)
	{
		MessageBoxA(NULL,"This is not a Lib!","Error",MB_ICONWARNING);
		fclose(pFile);
		return FALSE;
	}

	fclose(pFile);
	return TRUE;
}

BOOL CLibParser::InitOutPutFile(PCSTR szLib)
{
	int strSize=strlen(szLib)+1;
	if (strSize>MAX_PATH)
	{
		MessageBoxA(NULL,"File name is too long !","Error",MB_ICONWARNING);
		return FALSE;
	}
	
	strcpy_s(m_FlibFileName,strSize,szLib);
	ModifySuffix(m_FlibFileName,".flb");
	if(!fopen_S(&m_pFlibFile,m_FlibFileName,"wb"))
		return FALSE;
	strcpy_s(m_NameFileName,strSize,szLib);
	ModifySuffix(m_NameFileName,".nam");
	if(!fopen_S(&m_pNameFile,m_NameFileName,"wb"))
		return FALSE;
	strcpy_s(m_DataFileName,strSize,szLib);
	ModifySuffix(m_DataFileName,".dat");
	if(!fopen_S(&m_pDataFile,m_DataFileName,"wb"))
		return FALSE;

	return TRUE;
}

PBYTE CLibParser::GetFirstObjSection()
{
	int iCtrl=0;
	
	PBYTE pSect = m_pLibImage+IMAGE_ARCHIVE_START_SIZE;
	if(!pSect)return NULL;
	while(pSect)
	{
		
		if(memcmp(((PIMAGE_ARCHIVE_MEMBER_HEADER)pSect)->Name,IMAGE_ARCHIVE_LINKER_MEMBER,16)==0)
		{
			//Nothing
		}
		
		else if(memcmp(((PIMAGE_ARCHIVE_MEMBER_HEADER)pSect)->Name,IMAGE_ARCHIVE_LONGNAMES_MEMBER,16)==0)//LONG Name
		{
			//Nothing
			
		}	
		else //First Obj Section
		{
			return pSect;
		}
	
		PIMAGE_ARCHIVE_MEMBER_HEADER pAME=(PIMAGE_ARCHIVE_MEMBER_HEADER)pSect;
		pSect += atol((LPSTR)pAME->Size) + sizeof(IMAGE_ARCHIVE_MEMBER_HEADER);
		
		if(*pSect=='\n') pSect++;

		iCtrl++;
		if (iCtrl>3)
		{
			break;
		}
	}
	return NULL;
}


BOOL CLibParser::bImportlibraryFormat(PBYTE pSect)
{
	WORD Sig1=*(PWORD)(pSect);
	WORD Sig2=*(PWORD)(pSect+2);

	if (Sig1==IMAGE_FILE_MACHINE_UNKNOWN && Sig2==0xffff)
	{
		return TRUE;
	} 
	else
	{
		return FALSE;
	}
}


BOOL CLibParser::fopen_S(FILE ** _File, PCSTR _Filename,PCSTR _Mode)
{
	fopen_s(_File,_Filename,_Mode);

	if(*_File==NULL)
	{
		CHAR szError[MAX_PATH];
		sprintf_s(szError,MAX_PATH,"Can't Open %s",_Filename);
		MessageBoxA(NULL,szError,"Error",MB_ICONWARNING);
		return FALSE;
	}
	return TRUE;
}


void CLibParser::ModifySuffix(PCHAR filename,PCHAR pSuffix)
{
	PCHAR pDest=strrchr(filename,'.');
	do 
	{
		*pDest++=*pSuffix++;
	} while (*pDest&&*pSuffix);

	*pDest=0;
}
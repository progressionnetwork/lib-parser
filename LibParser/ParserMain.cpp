// LibParser.cpp 
//

#include <stdio.h>
#include "LibParser.h"


int main(int argc, CHAR* argv[])
{
	PCSTR szLib[]=
	{
		"..\\Libs\\VC6Lib\\libc.lib",
		"..\\Libs\\VC6Lib\\libcd.lib",

		"..\\Libs\\VC2003Lib\\libc.lib",
		"..\\Libs\\VC2003Lib\\libcd.lib",

		"..\\Libs\\VC2005Lib\\libcmt.lib",
		"..\\Libs\\VC2005Lib\\libcmtd.lib",

		"..\\Libs\\VC2008Lib\\libcmt.lib",
		"..\\Libs\\VC2008Lib\\libcmtd.lib",

		"..\\Libs\\VC2010_32\\libcmt.lib",
		"..\\Libs\\VC2010_32\\libcmtd.lib",
		"..\\Libs\\VC2010_64\\libcmt.lib",
		"..\\Libs\\VC2010_64\\libcmtd.lib",

		"..\\Libs\\VC2012_32\\libcmt.lib",
		"..\\Libs\\VC2012_64\\libcmt.lib",

		"..\\Libs\\VC2015_32\\libcmt.lib",
		"..\\Libs\\VC2015_32\\libcmtd.lib",
		"..\\Libs\\VC2015_64\\libcmt.lib",
		"..\\Libs\\VC2015_64\\libcmtd.lib",

		"..\\Libs\\VCDDK_32\\libcmt.lib",
		"..\\Libs\\VCDDK_64\\libcmt.lib",

		"..\\Libs\\VCWDK_32\\libcmt.lib",
		"..\\Libs\\VCWDK_64\\libcmt.lib",
	};
	
	const unsigned int num = sizeof(szLib)/sizeof(PCSTR);
	for (unsigned int i=0;i<num;i++)
	{
		CLibParser g_Parser;
		if(g_Parser.Parse(szLib[i]))
			printf("Parse Lib %s Succeed!\n",szLib[i]);
		else 
			printf("Parse Lib %s Failed!\n",szLib[i]);
	}

	getchar();

	return 0;
}


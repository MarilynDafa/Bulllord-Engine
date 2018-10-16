#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Console.h"
#include <vector>
#include <string>
/*
printf("this is an bpk package tool, you can make package , patch and so on\n");
printf("input -exec pack $folder$(dragable) $packagename$ $version$\n");
printf("eg : -exec pack D:/test de.bpk 1.20\n");
printf("input -exec patch $srcpackage$ $dstpackage$, build a patch\n");
printf("eg : -exec patch io120.bpk io130.bpk\n");
printf("input -exec query $packagename$, to show the information\n");
printf("eg : -exec query io.bpk or -exec query patch150.pat\n");
printf("input -help will show the help information\n");
printf("input -exit will exit BPKPackper\n");
printf("\n");
*/
int main(int argc, char* argv[])
{
	std::vector<std::string> cmds;
	
	for (int i = 0; i < argc; ++i)
	{
		cmds.push_back(argv[i]);
	}
	Console con;
	con._exec_command(cmds);
	return 0;
}

#include "Console.h"
#include <QEvent>
#include <fstream>
#include "zlib.h"
enum e_console_result
{
	CR_VOID_CMD,
	CR_PRINT_HELP,
	CR_INVALID_CMD,
	CR_COMMAND_FAIL,
	CR_COMMAND_SUC
};

static int tokenize(const std::string& str2, const std::string& whitespace_, std::vector<std::string>& outtokens_)
{
	std::string str = str2;
	char* ptr = (char*)str.c_str();
	char* token;
	while (nullptr != (token = strtok(ptr, whitespace_.c_str())))
	{
		outtokens_.push_back(token);
		ptr = nullptr;
	}
	return outtokens_.size();
}


static void trim(std::string& str)
{
	char ch_ = ' ';
	for (int i = 0; i < str.length(); ++i)
	{
		char code = *(str.c_str() + i);
		if (code == ch_ || code == '\n' || code == '\r' || code == '\t')
			str.erase(0, 1);
		else
			break;
	}
	for (int i = str.length(); i >= 0; --i)
	{
		char code = *(str.c_str() + i - 1);
		if (code == ch_ || code == '\n' || code == '\r' || code == '\t')
			str.erase(str.length() - 1, 1);
		else
			break;
	}
}


void Console::run()
{
	char str[4096] = { 0 };
	printHelp();
	while (true)
	{
		memset(str, 0, sizeof(str));
		fgets(str, 4096, stdin);
		if (processCmd(str))
			break;
	}
	emit finished();
}

void Console::printHelp()
{
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
}


bool Console::processCmd(const char* cmd)
{
	e_console_result cr;
	std::string str_ = cmd;
	if (str_.length() == 1)
		cr = CR_VOID_CMD;
	std::vector<std::string> args;
	tokenize(str_, " ", args);
	for (int i = 0; i < args.size(); ++i)
		trim(args[i]);
	if (args[0] == "-exec")
	{
		cr = _exec_command(args) ? CR_COMMAND_SUC : CR_COMMAND_FAIL;
		return false;
	}
	else if (args[0] == "-help")
	{
		printHelp();
		cr = CR_PRINT_HELP;
		return false;
	}
	else if (args[0] == "-exit")
		return true;
	else
	{
		error("invalid command");
		cr = CR_INVALID_CMD;
		return false;
	}
}

void Console::print_info(const std::string& context_)
{
	printf(">INFO:%s\n", context_.c_str());
}

void Console::error(const std::string& context_)
{
	printf(">ERROR: %s\n", context_.c_str());
}

bool Console::_exec_command(const std::vector<std::string>& args_)
{
	if (args_[1] == "pack")
	{
	}
	else if (args_[1] == "patch")
	{

	}
	else if (args_[1] == "query")
	{
	}
	else
	{
		error("invalid command");
		return false;
	}
	return true;
}


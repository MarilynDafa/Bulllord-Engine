#include "BMGTexTool.h"
#include <QtWidgets/QApplication>

#ifdef WIN32R
#include <QTime> 
#include <qmessagebox.h>
#include <Windows.h>
#include<DbgHelp.h>
LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException) {//程式异常捕获
															  /*
															  ***保存数据代码***
															  */
															  //创建 Dump 文件
	HANDLE hDumpFile = CreateFile((LPCWSTR)L"BMGTexTool-Crush.dmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDumpFile != INVALID_HANDLE_VALUE) {
		//Dump信息
		MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
		dumpInfo.ExceptionPointers = pException;
		dumpInfo.ThreadId = GetCurrentThreadId();
		dumpInfo.ClientPointers = TRUE;
		//写入Dump文件内容
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
	}
	//这里弹出一个错误对话框并退出程序
	EXCEPTION_RECORD* record = pException->ExceptionRecord;
	QString errCode(QString::number(record->ExceptionCode, 16)), errAdr(QString::number((uint)record->ExceptionAddress, 16)), errMod;
	QMessageBox::critical(NULL, "BMGTexTool Crush", "<FONT size=4><div><b>Create this fucking app's crush log</b><br/></div>" +
		QString("<div>Error Code：%1</div><div>Error Address：%2</div></FONT>").arg(errCode).arg(errAdr),
		QMessageBox::Ok);
	return EXCEPTION_EXECUTE_HANDLER;
}
#endif
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#ifdef WIN32R
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);//注冊异常捕获函数  
#endif
    BMGTexTool w;
    w.show();
    return a.exec();
}

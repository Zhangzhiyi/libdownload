#include "GlobalFunction.h"
#include "LogManager.h"
#include "PlatformDefine.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef WIN32
#include <windows.h>
#elif defined ANDROID
#include <android/log.h>
#endif

CDateFileLogger& GetMainLogger(void)
{
//	static const int MAIN_LOGGER_PRIORITY = GetPrivateProfileInt("main_logger", "priority", LOG_ERROR, "ini/info.ini");
	static CDateFileLogger s_mainLogger(LOG_ERROR);
	return s_mainLogger;
}

// =====================================================================================================================
//   str为要打开的文件名(包含路径) 例如 debug1/debug2/debug3/debug.log
// =====================================================================================================================
CFileLogWriter::CFileLogWriter(const std::string &str)
{
	// 先创建文件夹
	size_t pos = str.find('/');
	std::string strTemp;
	while (pos != std::string::npos) {
		strTemp = str.substr(0, pos);
		MyCreateDirectory(strTemp.c_str());
		pos = str.find('/', pos + 1);
	}

	m_fp = MyFileOpen(str.c_str(), "a+");
}

// =====================================================================================================================
// =======================================================================================================================
CFileLogWriter::~CFileLogWriter()
{
	if (m_fp) {
		fclose(m_fp);
		m_fp = NULL;
	}
}

// =====================================================================================================================
// =======================================================================================================================
void CFileLogWriter::Write(const std::string &str)
{
	if (m_fp) {

		//~~~~~~~~~
		time_t ltime;
		//~~~~~~~~~

		time(&ltime);
		fprintf(m_fp, "%s -- %s", str.c_str(), ctime(&ltime));

		// 直接调用fflush，则每条Log都直接写入到文件中，相当于屏蔽掉fopen系列函数的缓存
		// 当写的log比较多时，调用fflush比不调用慢2~4倍
		fflush(m_fp);
	}
}

// =====================================================================================================================
// =======================================================================================================================
CDateFileLogWriter::CDateFileLogWriter()
{
	//~~~~~~~~~
	time_t ltime;
	//~~~~~~~~~

	time(&ltime);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	struct tm *newtime = localtime(&ltime);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	char path[256] = {0};
	snprintf(path, sizeof(path), "%s/%s", GetDocumentPath(), "debug");

	MyCreateDirectory(path);

	//~~~~~~~~~~~~~~~~~~~~~
	char szLogName[256] = "";
	//~~~~~~~~~~~~~~~~~~~~~

	snprintf(szLogName, sizeof(szLogName) - 1, "%s/%u_%u_%u.log", path, newtime->tm_year + 1900, newtime->tm_mon + 1,
			  newtime->tm_mday);
	szLogName[sizeof(szLogName) - 1] = 0;
	m_strFileName = szLogName;
	m_fp = fopen(m_strFileName.c_str(), "a+");
}

// =====================================================================================================================
// =======================================================================================================================
CDateFileLogWriter::~CDateFileLogWriter()
{
	if (m_fp) {
		fclose(m_fp);
		m_fp = NULL;
	}
}

// =====================================================================================================================
// =======================================================================================================================
void CDateFileLogWriter::Write(const std::string &str)
{
	if (m_fp) {

		//~~~~~~~~~
		time_t ltime;
		//~~~~~~~~~

		time(&ltime);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		struct tm *newtime = localtime(&ltime);
		char szLogName[256] = "";
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// 检查日期是否已改变，如果改变，则创建一个新的文件
		snprintf(szLogName, sizeof(szLogName) - 1, "%s/debug/%u_%u_%u.log", GetDocumentPath(), newtime->tm_year + 1900, newtime->tm_mon + 1,
			newtime->tm_mday);
		szLogName[sizeof(szLogName) - 1] = 0;

		if (m_strFileName != szLogName) {
			fclose(m_fp);
			m_strFileName = szLogName;
			m_fp = fopen(m_strFileName.c_str(), "a+");
		}

		if (m_fp) {
#if defined DEBUG || defined _DEBUG
			char szBuf[1024] = {0};
			snprintf(szBuf, 1024, "%s -- %s\n", str.c_str(), ctime(&ltime));
			DebugMsg(szBuf);
#endif
			fprintf(m_fp, "%s -- %s", str.c_str(), ctime(&ltime));
			fflush(m_fp);
		}
	}
}

void DebugMsg(const char* fmt...)
{
#if defined DEBUG || defined(_DEBUG) || defined(ANDROID)
	char szBuf[1024] = {0};

	va_list ap;
	va_start(ap, fmt);
#ifdef WIN32
	vsnprintf_s(szBuf, 1024, 1024, fmt, ap);
#else
    vsnprintf(szBuf, 1024, fmt, ap);
#endif
	va_end(ap);

#ifdef WIN32
	WCHAR wszBuf[1024] = {0};
	MultiByteToWideChar(CP_UTF8, 0, szBuf, -1, wszBuf, sizeof(wszBuf));
	OutputDebugStringW(wszBuf);
	OutputDebugStringW(L"\n");
#elif defined ANDROID
	__android_log_print(ANDROID_LOG_DEBUG, "wing debug info",  szBuf);
#else
	printf("%s\n", szBuf);
#endif

#endif
}

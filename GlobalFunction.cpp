#ifdef WIN32
#include <direct.h>
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#endif

#include "PlatformDefine.h"
#include "StringFormat.h"

char g_document_path[1024] = {0};

#ifndef __APPLE__
CC_EXTERN const char* GetDocumentPath()
{
#ifdef WIN32
	if (g_document_path[0] == 0)
	{
		g_document_path[0] = '.';
	}

	return g_document_path;
#elif defined ANDROID
	if (g_document_path[0] == 0) {
		// define in cocos2dx JNIHelper.cpp
		extern const char* getPackageName();
		snprintf(g_document_path, sizeof(g_document_path), "/mnt/sdcard/data/%s", getPackageName());

		if (access(g_document_path, R_OK) != 0) {
			MyCreateDirectory(g_document_path);
		}
	}

	return g_document_path;
#endif
}
#endif


// 精确到毫秒
unsigned int TimeGet(void)
{
#ifdef WIN32
	return timeGetTime();
#else
	struct timeval start;
	gettimeofday(&start, NULL);
	unsigned int dwTime = start.tv_sec * 1000 + start.tv_usec / 1000;
	return dwTime;
#endif
}

#ifdef WIN32
LARGE_INTEGER g_count_start;
LARGE_INTEGER g_cpu_frequent;
#endif

// 精确到微妙
int64 TimeGetEx(void)
{
#ifdef WIN32
	int64 dida;
	LARGE_INTEGER end;
	QueryPerformanceCounter(&end);
	dida = end.QuadPart - g_count_start.QuadPart;
	double times = (double)dida / (double)g_cpu_frequent.QuadPart;
	int64 timen = (int64)(times * 1000000);
	return timen;
#else
	struct timeval start;
	gettimeofday(&start, NULL);
	int64 dwTime = (int64)start.tv_sec % 10000 * 1000000 + start.tv_usec;
	return dwTime;
#endif
}

#ifndef __APPLE__
// =====================================================================================================================
// =======================================================================================================================
CC_EXTERN int MyCreateDirectory(const char *pszDirectory)
{
	if (!pszDirectory || pszDirectory[0] == 0) {
		return 0;
	}
    
    if (access(pszDirectory, 0) == 0) {
        return 1;
    }

    int ret;
    char tempBuffer[512] = {0};
    strncpy(tempBuffer, pszDirectory, sizeof(tempBuffer) - 1);
    int len = strlen(pszDirectory);
    if (len < sizeof(tempBuffer) - 1 && tempBuffer[len - 1] != '/' && tempBuffer[len - 1] != '\\') {
        tempBuffer[len] = '/';
		++len;
    }
    
    for (int i = 1; i < len; ++i) {
        if (tempBuffer[i] == '\\' || tempBuffer[i] == '/') {
            tempBuffer[i] = 0;
            
            ret = access(tempBuffer, 0);
            if (ret != 0) {
#ifdef WIN32
                ret = mkdir(tempBuffer);
#else
                ret = mkdir(tempBuffer, S_IRUSR | S_IWUSR | S_IXUSR);
#endif
                if (ret != 0) {
                    return 0;
                }
            }
            
            tempBuffer[i] = '/';
        }
    }

    return 1;
}
#endif

// =====================================================================================================================
// =======================================================================================================================
CC_EXTERN void MyDeleteFile(const char *pszDirectory)
{
	remove(pszDirectory);
}

#if defined(WIN32)
void LogMsgError(const char* pszErrorType, const char* pszCallStacks, const char* pszExtractInfo)
{
}

void SendErrorLogToHttp(const char* pszLog)
{
}


CC_EXTERN void SendEventToAdx(const char* event, const char* data)
{
}

CC_EXTERN FILE* MyFileOpen(const char* pszFile, const char* pszMode)
{
	return fopen(pszFile, pszMode);
}
#elif defined(ANDROID)
CC_EXTERN void SendEventToAdx(const char* event, const char* data)
{
}

CC_EXTERN FILE* MyFileOpen(const char* pszFile, const char* pszMode)
{
	return fopen(pszFile, pszMode);
}
#endif
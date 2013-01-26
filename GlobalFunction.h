#pragma once

#include <stdio.h>

#ifdef __cplusplus
#define CC_EXTERN extern "C"
#else
#define CC_EXTERN extern
#endif

// 功能函数封装
CC_EXTERN int MyCreateDirectory(const char* pszDirectory);						// 创建目录。
CC_EXTERN void MyDeleteFile(const char* pszDirectory);							// 删除文件
CC_EXTERN FILE* MyFileOpen(const char* pszFile, const char* pszMode);				// 打开文件。用此函数代替fopen
CC_EXTERN void LogMsgError(const char* pszErrorType, const char* pszCallStacks, const char* pszExtractInfo = NULL);
CC_EXTERN void SendErrorLogToHttp(const char* pszLog);
CC_EXTERN const char* GetDocumentPath();
CC_EXTERN const char* GetApplicationPath();
CC_EXTERN unsigned int TimeGet(void);

CC_EXTERN void SendEventToAdx(const char* event, const char* data);
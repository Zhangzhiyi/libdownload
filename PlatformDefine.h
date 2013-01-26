#pragma once
#ifdef __cplusplus
#include "LogManager.h"
#include "GlobalFunction.h"

#ifdef WIN32
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#else
#define _snprintf snprintf
#define stricmp strcasecmp
#endif

#ifndef __max
#	define __max(a, b)         ((a) > (b)? (a) : (b))
#endif

#ifndef __min
#	define __min(a, b)         ((a) < (b)? (a) : (b))
#endif

//////////////////////////////////////////////////////////////////////////
//#define uint unsigned int
#ifndef uint
typedef unsigned int uint;
#endif//uint

#ifndef uint8 
typedef unsigned char uint8;
#endif//uint8

#ifndef int8
typedef signed char int8;
#endif//int8

#ifndef uint16
typedef unsigned short uint16;
#endif//uint16

#ifndef int16
typedef signed short int16;
#endif//int16

#ifndef uint32
typedef unsigned int uint32;
#endif//uint32

#ifndef int32
typedef signed int int32;
#endif//int32

#ifndef uint64
typedef unsigned long long uint64;
#endif//uint64

#ifndef int64
typedef long long int64;
#endif//

#define ccTime float


inline void log_msg(const char *type,
	const char *str,
	const char *file,
	int line)
{
	char temp[256] = {0};
	snprintf(temp, sizeof(temp), "%s(%s) in %s, %d", type, str, file, line);
	LogMsg(temp);
}
#ifdef WIN32
	#define MY_ASSERT(cond) (void)((cond) || (log_msg("ASSERT", #cond, __FILE__, __LINE__), assert(cond), 0))
	#define MY_ASSERT_EX(cond, msg) (void)((cond) || (log_msg("ASSERT", #cond, __FILE__, __LINE__), assert(cond), 0))
#else
	#define MY_ASSERT(cond) (void)((cond) || (log_msg("ASSERT", #cond, __FILE__, __LINE__), 0))
	#define MY_ASSERT_EX(cond, msg) (void)((cond) || (log_msg("ASSERT", #cond, __FILE__, __LINE__), 0))
#endif

#define CHECKF(x) \
	do \
{ \
	if (!(x)) { \
	log_msg("CHECKF", #x, __FILE__, __LINE__); \
	return 0; \
	} \
} while (0)


#define SWAP16(x) static_cast(uint16, ((x << 8) | (x >> 8)));

//#define USE_MPQ_PACKAGE
//#deinfe USE_ZIP_PACKAGE
#define USE_ZPACK_PACKAGE 1


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <algorithm>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <cassert>

#ifdef WIN32
#include <windows.h>
#endif

#endif

#ifndef __LogManager_H__
#define __LogManager_H__

#include <sstream>
#include <stdarg.h>
#ifdef WIN32
#pragma warning(disable:4996)
#endif
#define OGRE_AUTO_MUTEX
#define OGRE_LOCK_AUTO_MUTEX

// 日志的等级 最常用的两个等级是 LOG_ERROR 和 LOG_INFO，通过 ERROR_S来记录一些确实会引发问题的错误，通过 INFO_S来记录一些有用的调试信息
// 游戏中的最主要的日志记录类 MainLogger可以通过info.ini中的配置来改变优先级来调整输出的内容，而不需要重新编译客户端
// 默认情况下，其等级是 LOG_ERROR，即通过ERROR_S 记录的日志会被输出，而通过 INFO_S 记录的日志不会被输出
// info.ini中  
// [main_logger]
// priority=300
// 通过增加这样的配置就可以达到同时输出 INFO_S记录的日志的目的
enum { LOG_FATAL = 0, LOG_ERROR = 100, LOG_WARN = 200, LOG_INFO = 300, LOG_DEBUG = 400, };

// 真正用于记录日志的writer
// CFileLogWriter可以指定输出文件
class CFileLogWriter
{
public:
	CFileLogWriter(const std::string &str);
	
	virtual ~CFileLogWriter();
	virtual void Write(const std::string &str);
protected:
	FILE* m_fp;
};

// CDateFileLogWriter根据日期自动生成输出文件
class CDateFileLogWriter
{
public:
	CDateFileLogWriter();
	
	virtual ~CDateFileLogWriter();
	virtual void Write(const std::string &str);
protected:
	std::string m_strFileName;
	FILE* m_fp;
};

template<typename WriterType>
class CLogger
{
protected:
	int m_nLogPriority;
	WriterType m_LogWriter;
public:
	OGRE_AUTO_MUTEX // public to allow external locking
	// 用于CDateFileLogWriter的构造，不需要传string参数
	CLogger(int nPriority = LOG_ERROR) :
	m_nLogPriority(nPriority)
	{
	}

	// 用于CFileLogWriter的构造，需要传输出文件名的参数
	CLogger(const std::string &strFileName, int nPriority = LOG_ERROR) :
	m_nLogPriority(nPriority),
	m_LogWriter(strFileName)
	{
	}

	~CLogger()
	{
	}

	// 几个记录日志的接口
	void LogMsg(int nPriority, const std::string &message)
	{
		if (this->IsPriorityEnabled(nPriority)) {
			m_LogWriter.Write(message);
		}
	}

	void LogMsg(int nPriority, const char *fmt, ...)
	{
		if (this->IsPriorityEnabled(nPriority)) {

			//~~~~~~~~~~~~~~~~~~~
			char buffer[1024] = "";
			//~~~~~~~~~~~~~~~~~~~
			
			va_list ap;
			va_start(ap, fmt);
			vsnprintf(buffer, sizeof(buffer) - 1, fmt, ap);
			va_end(ap);
			buffer[sizeof(buffer) - 1] = 0;
			m_LogWriter.Write(buffer);
		}
	}

	void ErrorMsg(const char *fmt, ...)
	{
		if (fmt && this->IsPriorityEnabled(LOG_ERROR)) {
			char buffer[1024] = "";
			va_list ap;
			va_start(ap, fmt);
			vsnprintf(buffer, sizeof(buffer) - 1, fmt, ap);
			va_end(ap);
			buffer[sizeof(buffer) - 1] = 0;
			m_LogWriter.Write(buffer);
		}
	}

	void InfoMsg(const char *fmt, ...)
	{
		if (fmt && this->IsPriorityEnabled(LOG_INFO)) {

			char buffer[1024] = "";
			va_list ap;
			va_start(ap, fmt);
			vsnprintf(buffer, sizeof(buffer) - 1, fmt, ap);
			va_end(ap);
			buffer[sizeof(buffer) - 1] = 0;
			m_LogWriter.Write(buffer);
		}
	}

	// 判断一条日志是否需要被输出，数值越低则优先级越高，当一条日志的优先级小于等于logger的优先级，则此条日志会被输出
	bool IsPriorityEnabled(int nPriority) const
	{
		return m_nLogPriority >= nPriority;
	}

	void SetLogPriority(int nPriority)
	{
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		OGRE_LOCK_AUTO_MUTEX m_nLogPriority = nPriority;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	}

	int GetLogPriority() const
	{
		return m_nLogPriority;
	}

	// 日志的记录支持流的方式，在流析构的时候会将日志内容写入文件。所以当 ERROR_S << 1 << 2 << 3;  当这一系列输入执行完毕，日志就会记录。
	class Stream;
	Stream GetStream(int nPriority)
	{
		return Stream(this, nPriority);
	}

	class Stream
	{
	protected:
		CLogger *m_pLogger;
		int m_nPriority;
		std::ostringstream m_streamCache;
	public:
		Stream(CLogger * pLogger, int m_nPriority)
		:
		m_pLogger(pLogger),
		m_nPriority(m_nPriority) {
		}

		Stream (const Stream &rhs)
		:
		m_pLogger(rhs.m_pLogger),
		m_nPriority(rhs.m_nPriority) {
			m_streamCache.str(rhs.m_streamCache.str());
		}

		~Stream() {
			if (m_streamCache.tellp() > 0) {
				m_pLogger->LogMsg(m_nPriority, m_streamCache.str());
			}
		}

		template<typename T>
		Stream &operator<<(const T &v)
		{
			m_streamCache << v;
			return *this;
		}

		// 支持std的流操纵子(为了不引入iomanip这样的附加头文件，暂只支持ios_base中定义的操纵子，比如std::hex这样的进制控制，或者std::left这样的位置控制)
		// 通过操纵子进行的设置在这个流被析构前会一直有效
		typedef std::ios_base& (*ios_base_fun)(std::ios_base&);
		Stream & operator << (std::ios_base& (*ios_base_fun)(std::ios_base&))
		{
			m_streamCache << ios_base_fun;
			return *this;
		}
	};
};

// 如果有新的writer，可以像这样定义一个新类型
typedef CLogger<CFileLogWriter> CFileLogger;
typedef CLogger<CDateFileLogWriter> CDateFileLogger;

extern CDateFileLogger &GetMainLogger(void);

// 以流的方式记录日志
// 优点是类型安全，缺点是当需要大量格式化操作时代码不如printf类操作来的简洁
// 当只进行简单的日志记录时推荐使用
// 使用举例:
// LOG_MSG_S(LOG_FATAL) << "发生致命错误";
// ERROR_S << "文件读取失败" << file_name;
// INFO_S << "接受到服务器的参数: " << i << "par2 = " << j; 
#define LOG_MSG_S(priority) \
	if (!GetMainLogger().IsPriorityEnabled(priority)); \
	else GetMainLogger().GetStream(priority)
#define ERROR_S if (!GetMainLogger().IsPriorityEnabled(LOG_ERROR)); \
	else GetMainLogger().GetStream(LOG_ERROR)
#define INFO_S	if (!GetMainLogger().IsPriorityEnabled(LOG_INFO)); \
	else GetMainLogger().GetStream(LOG_INFO)

// 支持变参
// 使用举例:
// LOG_MSG(LOG_WARN, "警告: %d %d", i, j);
// LogMsg("错误: %d %d", i, j);
// INFO_MSG("信息: %d %d", i, j);
#define LOG_MSG		GetMainLogger().LogMsg
#define ERROR_MSG	if (!GetMainLogger().IsPriorityEnabled(LOG_ERROR)); \
	else GetMainLogger().ErrorMsg
#define INFO_MSG	if (!GetMainLogger().IsPriorityEnabled(LOG_INFO)); \
	else GetMainLogger().InfoMsg
#define LogMsg if (!GetMainLogger().IsPriorityEnabled(LOG_ERROR)); \
	else GetMainLogger().ErrorMsg

// 定义一个CFileLogger，主要用于输出log到一个自定义的文件中
// 在需要打Log的cpp文件中的全局变量区使用  DEFINE_LOGGER(mylogger, "debug/debug.log", 200)    类似这样的宏可以定义一个新的logger
// 其他需要使用这个logger的cpp文件中可以使用   DECLARE_LOGGER(mylogger)    类似这样的宏声明一个已有logger
// 需要记录日志的地方  使用 MY_ERROR_S(mylogger) << "log";
#define DEFINE_LOGGER(name, file_name, priority) \
	CFileLogger &Get##name##Logger() \
{	static CFileLogger s_logger(file_name, priority); \
	return s_logger; }

#define DECLARE_LOGGER(name) extern CFileLogger& Get##name##Logger();

#define MY_LOG_MSG_S(name, priority) if (!Get##name##Logger().IsPriorityEnabled(priority)) ; else Get##name##Logger().GetStream(priority)
#define MY_ERROR_S(name) if (!Get##name##Logger().IsPriorityEnabled(LOG_ERROR)) ; else Get##name##Logger().GetStream(LOG_ERROR)
#define MY_INFO_S(name) if (!Get##name##Logger().IsPriorityEnabled(LOG_INFO)) ; else Get##name##Logger().GetStream(LOG_INFO)
#define MY_LOG_MSG(name) Get##name##Logger().LogMsg
#define MY_LogMsg(name) if (!Get##name##Logger().IsPriorityEnabled(LOG_ERROR)) ; else Get##name##Logger().ErrorMsg
#define MY_INFO_MSG(name) if (!Get##name##Logger().IsPriorityEnabled(LOG_INFO)) ; else Get##name##Logger().InfoMsg
            
extern void DebugMsg(const char* fmt...);
#endif

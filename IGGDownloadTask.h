#pragma once
#include "fd/delegate.hpp"

class IDownLoadEvent;

class IGGDownloadTask
{
public:
	IGGDownloadTask(int taskId);
	~IGGDownloadTask();

	void setSavePath( const char * lpSavePath );				// 设置文件保存路径
	void setSpeedLimit(int speedKB);
	void setLowSpeedLimit(int speedKB, int second);
	void setUrl(const char * lpDownUrl, int threadCount);	// 设置下载地址

	bool start();		// 真正开始下载

	int getFileLenth(const char *url);
	int getTaskId() const;
	std::string getFileName() const;
private:
	int m_taskId;

	void * m_curlhandle;
	FILE* m_fp;
	
	std::string m_downSavePath;

	long long m_fileSize;
	long long m_localFileSize;
	std::string m_fileName;
	std::string m_url;

public:
	fd::delegate<void(int, std::string, int)> EventDownloadStart;	// taskId filename and size
	fd::delegate<void(int, std::string, double, double)> EventDownloadProcess;//taskId filename cursize and maxsize
	fd::delegate<void(int, std::string, std::string)> EventDownloadOver;	//taskId filename  savepath
	fd::delegate<void(int, std::string, std::string, int)> EventDownloadError;	//taskId filename  and error_resaon error_code
};

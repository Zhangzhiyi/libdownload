#pragma once

#ifdef WIN32
#pragma comment(lib, "winmm")
#pragma comment(lib, "wsock32")
#pragma comment(lib, "Wldap32")
#pragma comment(lib, "Ws2_32")
#endif


struct ReportStatus {
	unsigned int dwDoing;		//下载个数
	unsigned int dwRuning;		//运行个数
	unsigned int dwPause;		//暂停个数
	unsigned int dwStop;		//停止个数
	unsigned int dwTaskCount;	//任务总数
	unsigned int dwDownRate;	//下载速率
};

class IDownLoadEvent
{
public:
	virtual ~IDownLoadEvent() {}

	//任务完成时发生 szSavePath是文件存放路径
	virtual void InvokeDownOver(unsigned int dwTaskNo,const char * szFileName,const char * szSavePath) = 0;
	//任务下载时发生  dwPercent是表示万分之几
	virtual void InvokeDownGuage(unsigned int dwTaskNo, double curSize, double totalSize) = 0;
	//任务出错时发生
	virtual void InvokeReportErr(unsigned int dwTaskNo,const char * szErrDemo,unsigned int dwErrCode) = 0;
	//报告下载状态  20ms/t 
	virtual void InvokeReportState() = 0;
	//任务开始时发生 
	virtual void InvokeDownStart(unsigned int dwTaskNo,unsigned int dwReasion) = 0;
	//任务停止时发生 
	virtual void InvokeDownStop(unsigned int dwTaskNo,unsigned int dwReasion) = 0;
	//任务关闭时发生 
	virtual void InvokeDownClose(unsigned int dwTaskNo,unsigned int dwReasion) = 0;
};

class IDownLoadEvent;
class IGGDownloadTask;
class IGGDownloadSystem
{
public:
	IGGDownloadSystem();
	~IGGDownloadSystem();

	static IGGDownloadSystem& getSingleton();

	int addDownTaskUrl(const char * lpDownUrl, int threadCount = 1, const char* destPath = NULL);
	void setDownSavePath(const char * lpSavePath);
	void setDownEvent(IDownLoadEvent* pEvt);

	void setDownSpeedLimit(int speedKB);
	void setLowSpeedLimit(int speedKB, int second);

	std::string getTaskFileName(int taskId) const;
private:
	void onTaskBegin(int taskId, std::string fileName, int fileSize);
	void onTaskProcess(int taskId, std::string fileName, double cur, double total);
	void onTaskOver(int taskId, std::string fileName, std::string savePath);
	void onTaskError(int taskId, std::string fileName, std::string errInfo, int errCode);

	IDownLoadEvent* m_event;
	std::string m_savePath;

	int m_lowSpeedLimit;
	int m_lowSpeedTime;
	int m_speedLimit;
	int m_currentHandleId;			// 用于分配Task Id
	std::vector<IGGDownloadTask*> m_tasks;
};
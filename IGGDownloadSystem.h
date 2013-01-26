#pragma once

#ifdef WIN32
#pragma comment(lib, "winmm")
#pragma comment(lib, "wsock32")
#pragma comment(lib, "Wldap32")
#pragma comment(lib, "Ws2_32")
#endif


struct ReportStatus {
	unsigned int dwDoing;		//���ظ���
	unsigned int dwRuning;		//���и���
	unsigned int dwPause;		//��ͣ����
	unsigned int dwStop;		//ֹͣ����
	unsigned int dwTaskCount;	//��������
	unsigned int dwDownRate;	//��������
};

class IDownLoadEvent
{
public:
	virtual ~IDownLoadEvent() {}

	//�������ʱ���� szSavePath���ļ����·��
	virtual void InvokeDownOver(unsigned int dwTaskNo,const char * szFileName,const char * szSavePath) = 0;
	//��������ʱ����  dwPercent�Ǳ�ʾ���֮��
	virtual void InvokeDownGuage(unsigned int dwTaskNo, double curSize, double totalSize) = 0;
	//�������ʱ����
	virtual void InvokeReportErr(unsigned int dwTaskNo,const char * szErrDemo,unsigned int dwErrCode) = 0;
	//��������״̬  20ms/t 
	virtual void InvokeReportState() = 0;
	//����ʼʱ���� 
	virtual void InvokeDownStart(unsigned int dwTaskNo,unsigned int dwReasion) = 0;
	//����ֹͣʱ���� 
	virtual void InvokeDownStop(unsigned int dwTaskNo,unsigned int dwReasion) = 0;
	//����ر�ʱ���� 
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
	int m_currentHandleId;			// ���ڷ���Task Id
	std::vector<IGGDownloadTask*> m_tasks;
};
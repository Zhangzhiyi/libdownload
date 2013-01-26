#pragma once
#define ReportStatus ReportStatusEx
#define IDownLoadEvt IDownLoadEvtEx
#define AddDownTaskUrl AddDownTaskUrlEx
#define SetDownSavePath SetDownSavePathEx
#define StartDownTracker StartDownTrackerEx
#define CloseDownTracker CloseDownTrackerEx
#define SetDownLimitRate SetDownLimitRateEx

typedef struct _ReportStatus_{
	unsigned int dwDoing;		//下载个数
	unsigned int dwRuning;		//运行个数
	unsigned int dwPause;		//暂停个数
	unsigned int dwStop;		//停止个数
	unsigned int dwTaskCount;	//任务总数
	unsigned int dwDownRate;	//下载速率
}ReportStatus,*PReportStatus;

class IDownLoadEvt
{
public:
	virtual ~IDownLoadEvt() {}
    
	//任务完成时发生 szSavePath是文件存放路径
	virtual void InvokeDownOver(unsigned int dwTaskNo,const char * szFileName,const char * szSavePath) = 0;
	//任务下载时发生  dwPercent是表示万分之几
	virtual void InvokeDownGuageEx(unsigned int dwTaskNo,unsigned int dwPercent,unsigned int dwRate, int cur, int size) = 0;
	//任务出错时发生
	virtual void InvokeReportErr(unsigned int dwTaskNo,const char * szErrDemo,unsigned int dwErrCode) = 0;
	//报告下载状态  20ms/t
	virtual void InvokeReportState(ReportStatus status) = 0;
	//任务开始时发生
	virtual void InvokeDownStart(unsigned int dwTaskNo,unsigned int dwReasion) = 0;
	//任务停止时发生
	virtual void InvokeDownStop(unsigned int dwTaskNo,unsigned int dwReasion) = 0;
	//任务关闭时发生
	virtual void InvokeDownClose(unsigned int dwTaskNo,unsigned int dwReasion) = 0;
    
};

#if !defined WIN32
#ifndef __OBJC__
typedef signed char BOOL;
#endif
typedef unsigned char BYTE;
#define TRUE 1
#define FALSE 0
#define WINAPI
#endif

#define IGGSPEEDGET_API 

// 這裡引用原來IGGDown用到的接口,減少上層改動 [2/8/2012 SizhiHuang]
class IDownLoadEvt;
/**********************************目前有使用到的接口**************************************/
//全局设置
void StartDownTracker();
void CloseDownTracker();					// 關閉

BOOL SetDownEvent(IDownLoadEvt* pEvt);		//设置回调事件
BOOL SetDownSavePath(const char * lpSavePath);	//设置保存路径
BOOL SetDownMaxThread(unsigned int dwThreadCount);	//设置最大线程
BOOL SetRunMaxTaskCount(unsigned int dwTaskCount);	//设置最大任务数
BOOL SetDownLimitRate(unsigned int dwDownMaxRate);	//设置最高限速
BOOL SetDownTaskLevel(unsigned int dwTaskNo, BYTE cLevel);	//设置任务级别
//添加完整URL任务指令
unsigned int AddDownTaskUrl(const char * lpDownUrl, unsigned int dwPktLen, BYTE cLevel,BOOL bCon = TRUE);


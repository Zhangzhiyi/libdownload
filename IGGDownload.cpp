#include "axel/axel.h"
#include "PlatformDefine.h"
#include "IGGDownload.h"
#include <queue>
#include <semaphore.h>

static char g_downSavingPath[512] = {0};
static int g_maxSpeedLimit = 0;
static int g_currentThreadIndex = 0;
static bool g_exitThread = false;


static std::queue<DownloadParam*> g_downloadQueue;      // 下载队列
static pthread_mutex_t g_downloadMutex;
static pthread_t g_downloadingThread;
static sem_t* g_pSem = NULL;
static sem_t s_sem;
static unsigned int g_currentDownloadCount;

#define kSemName "downloading"

static IDownLoadEvtEx* g_downEvent = NULL;


extern void start_download(const char* url, const char* saving_path, int max_count, int max_speed, int taskId);
extern void set_verbose(int ver);

void update_percent(int taskId, int max, int cur, double percent, double rate)
{
    if (g_downEvent) {
        g_downEvent->InvokeDownGuageEx(taskId, percent * 10000, rate, cur, max);
    }
}

void* start_download_thread(void* param)
{
    DownloadParam* downloadParam = NULL;
	while (true) {
		int semWaitRet = sem_wait(g_pSem);
        if (semWaitRet < 0) {
            // error
            break;
        }
        
        pthread_mutex_lock(&g_downloadMutex);
        if (g_downloadQueue.empty()) {
            pthread_mutex_unlock(&g_downloadMutex);
            if (g_exitThread) {
                g_exitThread = false;
                break;
            } else {
                continue;
            }
        } else {
            downloadParam = g_downloadQueue.front();
            g_downloadQueue.pop();
            pthread_mutex_unlock(&g_downloadMutex);
        }
        
        if (!downloadParam) {
            continue;
        }
        
        if (g_downEvent) {
            g_downEvent->InvokeDownStart(downloadParam->taskId, 1);
        }
        
        start_download(downloadParam->url.c_str(), downloadParam->saving_path.c_str(), downloadParam->max_count, downloadParam->max_speed, downloadParam->taskId);
        if (g_downEvent) {
            g_downEvent->InvokeDownOver(downloadParam->taskId, downloadParam->url.c_str(), downloadParam->saving_path.c_str());
        }
        delete downloadParam;
        downloadParam = NULL;
    }
    
    if (g_pSem != NULL) {
#ifdef __APPLE__
        sem_unlink(kSemName);
        sem_close(g_pSem);
#else
        sem_destroy(g_pSem);
#endif
        g_pSem = NULL;
    }
    
    return NULL;
}


unsigned int AddDownTaskUrl(const char * lpDownUrl, unsigned int dwPktLen, BYTE cLevel,BOOL bCon)
{
    if (g_pSem == NULL) {
#ifdef __APPLE__
        g_pSem = sem_open(kSemName, O_CREAT, 0644, 0);
        if (g_pSem == SEM_FAILED) {
            g_pSem = NULL;
            return -1;
        }
#else
        int semInitRet = sem_init(&s_sem, 0, 0);
        if (semInitRet < 0) {
            g_pSem = NULL;
            return -1;
        }
		g_pSem = &s_sem;
#endif
        pthread_mutex_init(&g_downloadMutex, NULL);
        pthread_create(&g_downloadingThread, NULL, start_download_thread, NULL);
        g_exitThread = false;
    }

    DownloadParam* param = new DownloadParam;
    param->taskId =  ++g_currentDownloadCount;
    param->max_count = 0;
    param->max_speed = g_maxSpeedLimit;
    param->url = lpDownUrl;
    param->saving_path = g_downSavingPath;

    pthread_mutex_lock(&g_downloadMutex);
    g_downloadQueue.push(param);
    pthread_mutex_unlock(&g_downloadMutex);
    
    sem_post(g_pSem);

    return g_currentDownloadCount;
}

BOOL SetDownEvent(IDownLoadEvt* pEvt)
{
    g_downEvent = pEvt;
    return TRUE;
}

BOOL SetDownSavePath(const char * lpSavePath)
{
    strncpy(g_downSavingPath, lpSavePath, sizeof(g_downSavingPath) - 1);
    if (access(g_downSavingPath, 0) != 0) {
        MyCreateDirectory(g_downSavingPath);
    }
    return 1;
}


void StartDownTracker()
{
    set_verbose(1);
}

// 结束下载线程
void CloseDownTracker()
{
    set_verbose(0);
    g_exitThread = true;
}

BOOL SetDownLimitRate(unsigned int dwDownMaxRate)
{
    g_maxSpeedLimit = dwDownMaxRate;
    return 1;
}
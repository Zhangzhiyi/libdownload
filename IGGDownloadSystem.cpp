#include "PlatformDefine.h"
#include "IGGDownloadSystem.h"
#include "IGGDownloadTask.h"
#include <boost/bind.hpp>

IGGDownloadSystem::IGGDownloadSystem()
{
	// 这个只调用一次
	m_currentHandleId = 0;
	m_event = NULL;
	m_speedLimit = 0;
	m_lowSpeedLimit = 0;
	m_lowSpeedTime = 0;
//	curl_global_init(CURL_GLOBAL_ALL);
}

IGGDownloadSystem::~IGGDownloadSystem()
{
//	curl_global_cleanup();

	for (std::vector<IGGDownloadTask*>::iterator itr = m_tasks.begin(); itr != m_tasks.end(); ++itr) {
		delete *itr;
	}
}

void IGGDownloadSystem::setDownSavePath( const char * lpSavePath )
{
	if (!lpSavePath || lpSavePath[0] == 0) {
		return;
	}

	m_savePath = lpSavePath;

	if (m_savePath[m_savePath.size() - 1] != '/') {
		m_savePath += "/";
	}
}

void IGGDownloadSystem::setDownEvent( IDownLoadEvent* pEvt )
{
	if (!pEvt) {
		return;
	}

	m_event = pEvt;
}

int IGGDownloadSystem::addDownTaskUrl( const char * lpDownUrl, int threadCount, const char* destPath)
{
	IGGDownloadTask* task = new IGGDownloadTask(++m_currentHandleId);
	if (!task) {
		return -1;
	}

	if (destPath && destPath[0] != 0) {
		task->setSavePath(destPath);
	} else {
		task->setSavePath(m_savePath.c_str());
	}
	
	if (m_speedLimit != 0) {
		task->setSpeedLimit(m_speedLimit);
	}

	if (m_lowSpeedLimit != 0 && m_lowSpeedTime != 0) {
		task->setLowSpeedLimit(m_lowSpeedLimit, m_lowSpeedTime);
	}

	task->setUrl(lpDownUrl, threadCount);

	task->EventDownloadStart = boost::bind(&IGGDownloadSystem::onTaskBegin, this, _1, _2, _3);
	task->EventDownloadProcess = boost::bind(&IGGDownloadSystem::onTaskProcess, this, _1, _2, _3, _4);
	task->EventDownloadOver = boost::bind(&IGGDownloadSystem::onTaskOver, this, _1, _2, _3);
	task->EventDownloadError = boost::bind(&IGGDownloadSystem::onTaskError, this, _1, _2, _3, _4);

	m_tasks.push_back(task);
	return task->getTaskId();
}

//void IGGDownloadSystem::removeTask( int taskId)
//{
//	for (std::vector<IGGDownloadTask*>::iterator itr = m_tasks.begin(); itr != m_tasks.end(); ++itr) {
//		if ((*itr) && (*itr)->getTaskId() == taskId) {
//			delete *itr;
//			m_tasks.erase(itr);
//			break;
//		}
//	}
//}
//
//void IGGDownloadSystem::processTaskQueue()
//{
//	if (!m_tasks.empty()) {
//		IGGDownloadTask* taskNext = m_tasks.front();
//		if (taskNext) {
//			taskNext->start();
//		}
//	} else {
//		if (m_event) {
//			m_event->InvokeReportState();
//		}
//	}
//}

void IGGDownloadSystem::setDownSpeedLimit( int speedKB )
{
	m_speedLimit = speedKB;
}

void IGGDownloadSystem::setLowSpeedLimit( int speedKB, int second )
{
	m_lowSpeedLimit = speedKB;
	m_lowSpeedTime = second;
}

IGGDownloadSystem& IGGDownloadSystem::getSingleton()
{
	static IGGDownloadSystem s_instance;
	return s_instance;
}

void IGGDownloadSystem::onTaskBegin(int taskId,  std::string fileName, int fileSize )
{
	if (m_event) {
		m_event->InvokeDownStart(taskId, fileSize);
	}
}

void IGGDownloadSystem::onTaskProcess( int taskId, std::string fileName, double cur, double total )
{
	if (m_event) {
		m_event->InvokeDownGuage(taskId, cur, total);
	}
}

void IGGDownloadSystem::onTaskOver(int taskId,  std::string fileName ,std::string savePath)
{
	if (m_event) {
		m_event->InvokeDownOver(taskId, fileName.c_str(), savePath.c_str());
	}
}

void IGGDownloadSystem::onTaskError(int taskId,  std::string fileName, std::string errInfo, int errCode )
{
	if (m_event) {
		m_event->InvokeReportErr(taskId, errInfo.c_str(), errCode);
	}
}

std::string IGGDownloadSystem::getTaskFileName( int taskId ) const
{
	for (std::vector<IGGDownloadTask*>::const_iterator itr = m_tasks.begin(); itr != m_tasks.end(); ++itr) {
		if ((*itr) && (*itr)->getTaskId() == taskId) {
			return (*itr)->getFileName();
		}
	}

	return "";
}

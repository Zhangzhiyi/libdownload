#include "PlatformDefine.h"
#include "IGGDownloadTask.h"
#include "IGGDownloadSystem.h"

#ifdef WIN32
#include <io.h>
#endif

// ��httpͷ����ȡ�ļ�size
size_t get_content_length_func(void *ptr, size_t size, size_t nmemb, void *stream)
{
	int r = 0;
	long len = 0;
	r = sscanf((char*)ptr, "Content-Length: %ld\n", &len);
	if (r) {
		*((long long*) stream) = len;
	}

	return size * nmemb;
}

/* ���������ļ� */
size_t wirte_func(void *ptr, size_t size, size_t nmemb, void *stream)
{
	return fwrite(ptr, size, nmemb, (FILE*)stream);
}

/*��ȡ�ϴ��ļ� */
size_t read_func(void *ptr, size_t size, size_t nmemb, void *stream)
{
	FILE *f = (FILE*)stream;
	size_t n;

	if (ferror(f))
		return -1;

	n = fread(ptr, size, nmemb, f) * size;

	return n;
}

//���������Ϊ�˷���CURLOPT_PROGRESSFUNCTION�������
//��ʾ�ļ�������ȣ�t�����ļ���С��d���� ���Ѿ���ɲ���
int my_progress_func(void* data, double t, /* dltotal */
	double d, /* dlnow */
	double ultotal,
	double ulnow)
{
	IGGDownloadTask* task = (IGGDownloadTask*)data;
	if (task && task->EventDownloadProcess) {
		task->EventDownloadProcess(task->getTaskId(), task->getFileName(), d, t);
	}
	return 0;
}

IGGDownloadTask::IGGDownloadTask(int taskId)
{
	m_taskId = taskId;
	m_fileSize = 0;
	m_fp = NULL;

	// ÿ�������Ӧһ��curl���
//	m_curlhandle = curl_easy_init();
}

IGGDownloadTask::~IGGDownloadTask()
{
//	if (m_curlhandle) {
//		// �����������������
//		curl_easy_cleanup(m_curlhandle);
//	}
}

int IGGDownloadTask::getTaskId() const
{
	return m_taskId;
}

std::string IGGDownloadTask::getFileName() const
{
	return m_fileName;
}

void IGGDownloadTask::setSpeedLimit( int speedKB )
{
	if (speedKB <= 0) {
		return;
	}

	long long s1 = speedKB * 1024;
	// MAX_SEND_SPEED_LARGE
//	curl_easy_setopt(m_curlhandle, CURLOPT_MAX_RECV_SPEED_LARGE, s1); // ������������ٶ�
}

void IGGDownloadTask::setLowSpeedLimit( int speedKB, int second )
{
	if (speedKB <= 0 || second <= 0) {
		return;
	}

//	long s1 = speedKB * 1024;
//	long s2 = second;
//	curl_easy_setopt(m_curlhandle, CURLOPT_LOW_SPEED_LIMIT, s1);
//	curl_easy_setopt(m_curlhandle, CURLOPT_LOW_SPEED_TIME, s2);
}

int IGGDownloadTask::getFileLenth(const char *url) {  
	long downloadFileLenth = 0;  
//	curl_easy_setopt(m_curlhandle, CURLOPT_URL, url);  
//	curl_easy_setopt(m_curlhandle, CURLOPT_HEADER, 1);    //ֻ��Ҫheaderͷ  
//	curl_easy_setopt(m_curlhandle, CURLOPT_NOBODY, 1);    //����Ҫbody  
//	if (curl_easy_perform(m_curlhandle) == CURLE_OK) {  
//		curl_easy_getinfo(m_curlhandle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &downloadFileLenth);  
//	} else {  
//		downloadFileLenth = -1;  
//	}  
	return downloadFileLenth;
}

void IGGDownloadTask::setUrl(const char * lpDownUrl, int threadCount)
{
#if 0
	if (!lpDownUrl || lpDownUrl[0] == 0) {
		return;
	}

	m_url = lpDownUrl;
	m_fileName = m_url.substr(m_url.rfind('/') + 1);
	std::string fullPath = m_downSavePath + m_fileName + ".tmp";
	
	CURLcode r = CURLE_GOT_NOTHING;
	int c;
	struct stat file_info;
	/* �õ������ļ���С */
	//if(access(localpath,F_OK) ==0)

	//����׷�ӷ�ʽ���ļ�������ʵ���ļ��ϵ���������
	m_fp = fopen(fullPath.c_str(), "ab+"); 
	if (m_fp == NULL) {
		return;
	}

	// ��ȡ�����ļ���С
	fseek(m_fp, 0, SEEK_END);
	m_localFileSize = ftell(m_fp);
	fseek(m_fp, 0, SEEK_SET);

	//curl_easy_setopt(curlhandle, CURLOPT_UPLOAD, 1L);

	curl_easy_setopt(m_curlhandle, CURLOPT_URL, m_url.c_str());

	curl_easy_setopt(m_curlhandle, CURLOPT_CONNECTTIMEOUT, 5);  // �������ӳ�ʱ����λ��

	//����http ͷ��������
	curl_easy_setopt(m_curlhandle, CURLOPT_HEADERFUNCTION, get_content_length_func);
	curl_easy_setopt(m_curlhandle, CURLOPT_HEADERDATA, &m_fileSize);

	if (m_localFileSize != 0) {
		// �����ļ�������λ�ø�libcurl ע�⴫�ݸ�curl����long long
		curl_easy_setopt(m_curlhandle, CURLOPT_RESUME_FROM_LARGE, m_localFileSize);
	}

	curl_easy_setopt(m_curlhandle, CURLOPT_WRITEDATA, m_fp);
	curl_easy_setopt(m_curlhandle, CURLOPT_WRITEFUNCTION, wirte_func);	// ���������ļ�д�뺯��

	//curl_easy_setopt(curlhandle, CURLOPT_READFUNCTION, read_func);	// �ϴ��ļ�ʹ��
	//curl_easy_setopt(curlhandle, CURLOPT_READDATA, f);
	curl_easy_setopt(m_curlhandle, CURLOPT_NOPROGRESS, 0);		// �ص����ؽ���
	curl_easy_setopt(m_curlhandle, CURLOPT_PROGRESSFUNCTION, my_progress_func);
	curl_easy_setopt(m_curlhandle, CURLOPT_PROGRESSDATA, this);

	curl_easy_setopt(m_curlhandle, CURLOPT_VERBOSE, 1);			// ��ʾ��ϸ��Ϣ
#endif
}

bool IGGDownloadTask::start()
{
#if 0
	if (EventDownloadStart) {
		EventDownloadStart(m_taskId, m_fileName, m_fileSize);
	}

	CURLcode r = curl_easy_perform(m_curlhandle);

	// ������ϣ��ر��ļ�
	fclose(m_fp);

	std::string fullPath = m_downSavePath + m_fileName + ".tmp";

	if (r == CURLE_OK) {
		std::string destPath = m_downSavePath + m_fileName;
		// ��ԭʼ�ļ�����ɾ��
		if (access(destPath.c_str(), 0) == 0) {
			remove(destPath.c_str());
		}

		rename(fullPath.c_str(), destPath.c_str());

		if (EventDownloadOver) {
			EventDownloadOver(m_taskId, m_fileName, m_downSavePath);
		}
	} else {
		if (EventDownloadError) {
			EventDownloadError(m_taskId, m_fileName, curl_easy_strerror(r), (int)r);
		}
	}

	return r == CURLE_OK;
#endif
	return 1;
}

void IGGDownloadTask::setSavePath( const char * lpSavePath )
{
	if (!lpSavePath || lpSavePath[0] == 0) {
		return;
	}

	m_downSavePath = lpSavePath;
	if (m_downSavePath[m_downSavePath.size() - 1] != '/') {
		m_downSavePath += "/";
	}

	if (access(m_downSavePath.c_str(), 0) != 0) {
		MyCreateDirectory(m_downSavePath.c_str());
	}
}

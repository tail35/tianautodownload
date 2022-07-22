#include "http_get.h"  
#include <shlwapi.h>
#include<windows.h>
#include <tchar.h>
#include "global.h"
#include <QtWidgets\QMessageBox>
#include <QTimer>

Http::Http()
{
}

Http::~Http()
{
	
}

BOOL CreateDirTree(LPCTSTR lpPath)
{

	if (NULL == lpPath || _tcslen(lpPath) == 0)
	{
		return FALSE;
	}

	if (::PathFileExists(lpPath) || ::PathIsRoot(lpPath))
		return TRUE;

	TCHAR szParentpath[MAX_PATH] = _T("");
	::lstrcpy(szParentpath, lpPath);

	::PathRemoveBackslash(szParentpath);//ȥ��·�����ķ�б��  
	::PathRemoveFileSpec(szParentpath);//��·��ĩβ���ļ������ļ��кͷ�б��ȥ��  

	if (0 == _tcscmp(lpPath, szParentpath))
		return FALSE;

	//assert(0 != _tcscmp(lpPath, szParentpath));
	if (CreateDirTree(szParentpath))//�ݹ鴴��ֱ����һ����ڻ��Ǹ�Ŀ¼  
	{
		return ::CreateDirectory(lpPath, NULL);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

void Http::httpDownload(download* parent,QString& strurl, QString& dir, QString& rdir, QString& uType) {

	mparent = parent;
	mrdir = rdir;
	//dir = "G:\\1.mp3";
	QString tmp = dir.replace("/", "\\");;

	QString onlyeDir = tmp.left( tmp.lastIndexOf("\\")+1 );

	QString fileName = tmp.right(tmp.length() - tmp.lastIndexOf("\\") - 1);

	bool res = CreateDirTree(onlyeDir.toStdWString().c_str());

	//str = "http://biaoqian.cn/t.php";
	
	//leInfo = url.path();
	//dir = "g:/1.txt";
	savedir = dir;
	file = new QFile(dir);
	QString hash;
	QString dirEncode = QUrl::toPercentEncoding(rdir);
	
	if (file->exists()) {
		QString hash = GetMd5(dir);
		hash = hash.toLower();
		strurl = QString("%1?hash=%2&rdir=%3&uType=%4&XDEBUG_SESSION_START=PHPSTORM").arg(strurl).arg(hash).arg(dirEncode).arg(uType);
		//res = file->open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::ReadOnly);//ֻд��ʽ���ļ�  		
	}
	else {
		strurl = QString("%1?hash=%2&rdir=%3&uType=%4&XDEBUG_SESSION_START=PHPSTORM").arg(strurl).arg("NEED_DOWN_IMEDIAE").arg(dirEncode).arg(uType);		
	}
	finalUrl = strurl;
	QUrl url(strurl);

	accessManager = new QNetworkAccessManager(this);
	request.setUrl(url);
	/******************����http��header***********************/
	// request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data");  
	// request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");  
	// request.setRawHeader("Content-Disposition","form-data;name='doc';filename='a.txt'");  
	//request.setHeader(QNetworkRequest::ContentLengthHeader,post_data.length());
	request.setHeader(QNetworkRequest::CookieHeader, "XDEBUG_SESSION=PHPSTORM");
	  
	//connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));//finishΪmanager�Դ����źţ�replyFinished���Զ����  
	
	reply = accessManager->get(request);//ͨ���������ݣ�����ֵ������replyָ����.

	//connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(onDownloadProgress(qint64, qint64)));//download�ļ�ʱ����
	//connect((QObject *)reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));		
	connect((QObject *)reply, SIGNAL(metaDataChanged()), this, SLOT(metaDataChanged()));
	QTimer::singleShot(10000, this, &Http::ConnectTimeOut);
	bConn = false;
	
}

void Http::ConnectTimeOut() {
	if (!bConn) {
		Finished(reply, DOWNLOAD_STATUS::DS_FAILD_NEED_REDOWN);
	}
}

/***************��Ӧ����**************************/
void Http::replyFinished(QNetworkReply *reply) {
	//��ȡ��Ӧ����Ϣ��״̬��Ϊ200��ʾ����  
	QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	//QByteArray bytes = reply->readAll();  //��ȡ�ֽ�
	//QString result(bytes);  //ת��Ϊ�ַ���
	//qDebug() << result;

	//�޴��󷵻�  
	if (reply->error() == QNetworkReply::NoError && 200== status_code)
	{		
		//return ok
		file->close();
		Finished(reply, DOWNLOAD_STATUS::DS_SUCESS_NEED_DELETE);
	}
	else
	{
		//redownload
		Finished(reply, DOWNLOAD_STATUS::DS_FAILD_NEED_REDOWN);
		//QString retVal;
		//QMetaObject::invokeMethod(mparent, "HttpDownloadFinishedCallBack", Qt::DirectConnection,
		//	Q_RETURN_ARG(QString, retVal),
		//	Q_ARG(QString, mrdir),
		//	Q_ARG(int, DOWNLOAD_STATUS::DS_FAILD_NEED_REDOWN)
		//);
		//IsContinue( QString(QString::fromLocal8Bit("���ش���%1")).arg(status_code.toString()), reply);
		////�������
		//if (400 == status_code.toInt()) {
		//}
	}

	reply->deleteLater();//Ҫɾ��reply�����ǲ�����repyfinished��ֱ��delete��Ҫ����deletelater;
	this->deleteLater();
}

void Http::Finished(QNetworkReply *reply, DOWNLOAD_STATUS ds) {
	
	QMetaObject::invokeMethod((QObject*)mparent, "HttpDownloadFinishedCallBack", Qt::QueuedConnection,		
		Q_ARG(QString, mrdir),
		Q_ARG(int, (int)ds)
	);
	//disconnect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
	//disconnect((QObject *)reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));	
	reply->deleteLater();
	reply->destroyed();
	accessManager->destroyed();
}

//void Http::IsContinue(QString title, QNetworkReply *reply) {
//	QMessageBox msgBox;
//	msgBox.setInformativeText(title);	
//	QPushButton *yesButton = msgBox.addButton(QString::fromLocal8Bit("���Դ��󣬼�������?"), QMessageBox::ActionRole);
//	QPushButton *cancelButton = msgBox.addButton(QString::fromLocal8Bit("�˳�?"), QMessageBox::ActionRole);
//	//QPushButton *abortButton = msgBox.addButton(QMessageBox::Abort);
//	msgBox.exec();
//	if (msgBox.clickedButton() == (QAbstractButton*)yesButton) {
//		Finished(reply);
//	}
//	else if (msgBox.clickedButton() == (QAbstractButton*)cancelButton) {
//		exit(0);
//	}
//}

void Http::metaDataChanged() {
	bConn = true;
	QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	int res = status_code.toInt();
	qDebug() << "start-------------";
	QList<QByteArray> headerList = reply->rawHeaderList();
	foreach(QByteArray head, headerList) {
		qDebug() << head << ":" << reply->rawHeader(head);
	}
	qDebug() << "end-------------end";
	if ( 200 == res && false == hasInit) {
		QByteArray qb = QString("code").toUtf8();
		bool ishas = reply->hasRawHeader(qb);
		if (ishas) {
			QByteArray code = reply->rawHeader(qb);
			QString str(code);
			int ncode = code.toInt();

			switch (ncode)
			{
			case HASH_PARAM_ERROR:				
			case HASH_SERVER_NO_FILE:
				{
					QString title;
					if (HASH_PARAM_ERROR == ncode) {
						title = QString("���ش��󣬲�������%1").arg(finalUrl);
					}
					else {
						title = QString("������û������ļ���") + mrdir ;
					}
					LogText(LogFileName, title);
					//IsContinue( title, reply);
				}				
			case HASH_EQUAL:
				{
					Finished(reply, DOWNLOAD_STATUS::DS_SUCESS_NEED_DELETE);
				}
				break;
			case HASH_WILL_DOWNLOAD:
				{
					bool res = file->open(QIODevice::Truncate | QIODevice::WriteOnly | QIODevice::ReadOnly);
					if (!res) {
						QString str = QString("�޷����ļ�,dir:%1").arg(savedir);
						//IsContinue(QString(QString::fromLocal8Bit("�޷����ļ�,dir:%1")).arg(savedir), reply);
						LogText(LogFileName, str);
					}
					connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
					connect((QObject *)reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
				}
				break;
			default:
				break;
			}
			int k = 0;
			k++;
		}
		else {
			Finished(reply, DOWNLOAD_STATUS::DS_FAILD_NEED_REDOWN);
			//IsContinue(QString(QString::fromLocal8Bit("û��codeͷ,dir:%1")).arg(mrdir), reply);

		}
	}
}

/***********���½�����*************/
void Http::onDownloadProgress(qint64 bytesSent, qint64 bytesTotal) {
	int k = 0;
	k++;
}

void Http::onReadyRead() {

	QByteArray ar = reply->readAll();
	qint64 size = ar.size(); 
	int tmp = ar.length();
	
	file->write(ar);	
	file->flush();
}

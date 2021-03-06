#ifndef HTTP_DOWNLOAD_H  
#define HTTP_DOWNLOAD_H  
#include <QtCore\QObject>
#include <QtCore\QFile>
#include <QtCore\QFileInfo>
#include <QtNetwork\QNetworkAccessManager>
#include <QtNetwork\QNetworkRequest>
#include <QtNetwork\QNetworkReply>
#include "global.h"
//#include "download.h"

class download;
class Http : public QObject
{
	Q_OBJECT
public:
	Http();
	~Http();
private:
	void Finished(QNetworkReply *reply, DOWNLOAD_STATUS ds);
	//void IsContinue(QString title,QNetworkReply *reply);
public slots:
	void ConnectTimeOut();
	void metaDataChanged();
	void httpDownload(download* parent,QString& strurl, QString& dir, QString& rdir, QString& uType);
	void replyFinished(QNetworkReply*reply);
	void onDownloadProgress(qint64 bytesSent, qint64 bytesTotal);
	void onReadyRead();
private:
	bool hasInit=false;
	QString mrdir;
	QString savedir;
	QString finalUrl;
	download* mparent;
	QFile *file;
	QFileInfo fileInfo;
	QNetworkAccessManager *accessManager;
	QNetworkRequest request;
	QNetworkReply *reply;
	bool bConn = false;
};



#endif
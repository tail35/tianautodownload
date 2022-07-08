#ifndef GLOBAL_H  
#define GLOBAL_H  
#include <QString>
#include <QtGlobal>
#include <QDebug>

const QString domain = "http://tlsbq.tlsbz.cn/";
//const QString domain = "http://biaoqian.cn/";
const QString expire = "30180901";
const QString advuri = "adv/adv.php";
const QString DOWN_FILE = "download.php";
const QString UPDATE_TXT = "update.txt";
const QString LogFileName = "log.txt";

enum DOWNLOAD_STATUS
{	
	DS_SUCESS_NEED_DELETE,//need remove item from download list.
	DS_FAILD_NEED_REDOWN, //need redownload the item.don't remove item from list.
	DS_DO_NOTHING
};
enum HASH_COMPARE
{	
	HASH_PARAM_ERROR=1,
	HASH_SERVER_NO_FILE=2,
	HASH_EQUAL = 3,//don't need download.
	HASH_WILL_DOWNLOAD=4
};

class FileObj {
public:
	QString rdir;
	qint64 size;
	QString hash;
};


void LogText(const QString fileName, const QString &msg);

const QString GetMd5(const QString& filePath);
bool IsDigitStr(QString src);

static qint64 total_size = 0;
static qint64 has_down_size = 0;

#endif
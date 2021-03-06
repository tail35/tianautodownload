#include "HWebView.h"
#include <QtGui\QDesktopServices>

HWebView::HWebView(QWidget *parent)
	: QWebEngineView(parent) {
	mParent = parent;
}

HWebView::~HWebView() {

}

void HWebView::Init(QUrl url) {
		
	this->load(QUrl(url));
	this->show();
	connect(this, SIGNAL(urlChanged(QUrl)), this, SLOT(onUrlChanged(QUrl)));
	//connect(this, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
}


void HWebView::onUrlChanged(QUrl url) {
	QString str = url.toString();
	
	if (-1 != str.indexOf("tian=111") ) {
		mOldUrl = url;
		this->back();
		LogText(LogFileName,QString("willopen:%1").arg(url.toString()));
		
		QDesktopServices::openUrl(url);
	}
}

QWebEngineView *HWebView::createWindow(QWebEnginePage::WebWindowType type) {
	//HWebView* view = new HWebView(mParent);
	//view->setAttribute(Qt::WA_DeleteOnClose);
	//view->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	//
	//return view;

	Q_UNUSED(type);
	return this;
}

void HWebView::loadFinished(bool ok) {
	int k = 0;
	k++;
}
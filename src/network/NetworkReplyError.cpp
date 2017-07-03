#include "NetworkReplyError.h"

using namespace governikus;


qint64 NetworkReplyError::readData(char* data, qint64 maxlen)
{
	Q_UNUSED(data);
	Q_UNUSED(maxlen);
	return -1;
}


void NetworkReplyError::onErrorSignals()
{
	Q_EMIT error(NetworkError::OperationCanceledError);
	Q_EMIT finished();
}


void NetworkReplyError::abort()
{
}


NetworkReplyError::NetworkReplyError(QNetworkRequest const& pRequest, QObject* pParent)
	: QNetworkReply(pParent)
{
	setRequest(pRequest);
	setError(NetworkError::OperationCanceledError, "Application shutting down");

	QMetaObject::invokeMethod(this, "onErrorSignals", Qt::QueuedConnection);
}

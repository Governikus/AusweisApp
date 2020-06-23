/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#include "NetworkReplyError.h"

using namespace governikus;


qint64 NetworkReplyError::readData(char* data, qint64 maxlen)
{
	Q_UNUSED(data)
	Q_UNUSED(maxlen)
	return -1;
}


void NetworkReplyError::onErrorSignals()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
	Q_EMIT errorOccurred(NetworkError::OperationCanceledError);
#else
	Q_EMIT error(NetworkError::OperationCanceledError);
#endif
	Q_EMIT finished();
}


void NetworkReplyError::abort()
{
}


NetworkReplyError::NetworkReplyError(QNetworkRequest const& pRequest, QObject* pParent)
	: QNetworkReply(pParent)
{
	setRequest(pRequest);
	setError(NetworkError::OperationCanceledError, QStringLiteral("Application shutting down"));

	QMetaObject::invokeMethod(this, &NetworkReplyError::onErrorSignals, Qt::QueuedConnection);
}

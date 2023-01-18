/*!
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#include "IfdConnectorImpl.h"

#include "Env.h"
#include "IfdDispatcher.h"
#include "SecureStorage.h"
#include "WebSocketChannel.h"

#include <QLoggingCategory>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	#include <QMutableListIterator>
#else
	#include <QMutableVectorIterator>
#endif


Q_DECLARE_LOGGING_CATEGORY(ifd)


namespace governikus
{
template<> IfdConnector* createNewObject<IfdConnector*>()
{
	return new IfdConnectorImpl();
}


} // namespace governikus


using namespace governikus;


void IfdConnectorImpl::removeRequest(const IfdDescriptor& pIfdDescriptor)
{
	QMutableVectorIterator<QSharedPointer<ConnectRequest>> requestIterator(mPendingRequests);
	while (requestIterator.hasNext())
	{
		const QSharedPointer<ConnectRequest> item = requestIterator.next();
		Q_ASSERT(item);
		if (item.isNull())
		{
			qCCritical(ifd) << "Unexpected null pending request";

			requestIterator.remove();
		}
		else if (item->getIfdDescriptor() == pIfdDescriptor)
		{
			requestIterator.remove();
		}
	}
}


void IfdConnectorImpl::onConnectionCreated(const IfdDescriptor& pIfdDescriptor,
		const QSharedPointer<QWebSocket>& pWebSocket)
{
	const QSharedPointer<DataChannel> channel(new WebSocketChannel(pWebSocket), &QObject::deleteLater);
	const IfdVersion::Version latestSupportedVersion = IfdVersion::selectLatestSupported(pIfdDescriptor.getApiVersions());
	const QSharedPointer<IfdDispatcherClient> dispatcher(Env::create<IfdDispatcherClient*>(latestSupportedVersion, channel), &QObject::deleteLater);

	removeRequest(pIfdDescriptor);

	Q_EMIT fireDispatcherCreated(pIfdDescriptor, dispatcher);
}


void IfdConnectorImpl::onConnectionError(const IfdDescriptor& pIfdDescriptor, const IfdErrorCode& pError)
{
	removeRequest(pIfdDescriptor);

	Q_EMIT fireDispatcherError(pIfdDescriptor, pError);
}


void IfdConnectorImpl::onConnectionTimeout(const IfdDescriptor& pIfdDescriptor)
{
	removeRequest(pIfdDescriptor);

	Q_EMIT fireDispatcherError(pIfdDescriptor, IfdErrorCode::CONNECTION_TIMEOUT);
}


IfdConnectorImpl::IfdConnectorImpl(int pConnectTimeoutMs)
	: mConnectTimeoutMs(pConnectTimeoutMs)
	, mPendingRequests()
{
}


void IfdConnectorImpl::onConnectRequest(const IfdDescriptor& pIfdDescriptor, const QString& pPsk)
{
	if (pIfdDescriptor.isNull() || pIfdDescriptor.getIfdName().isEmpty())
	{
		Q_EMIT fireDispatcherError(pIfdDescriptor, IfdErrorCode::INVALID_REQUEST);
		return;
	}

	// Currently, we only support API level 1.
	if (!pIfdDescriptor.isSupported())
	{
		Q_EMIT fireDispatcherError(pIfdDescriptor, IfdErrorCode::NO_SUPPORTED_API_LEVEL);
		return;
	}

	const QSharedPointer<ConnectRequest> newRequest(new ConnectRequest(pIfdDescriptor, pPsk.toUtf8(), mConnectTimeoutMs), &QObject::deleteLater);
	mPendingRequests += newRequest;
	connect(newRequest.data(), &ConnectRequest::fireConnectionCreated, this, &IfdConnectorImpl::onConnectionCreated);
	connect(newRequest.data(), &ConnectRequest::fireConnectionError, this, &IfdConnectorImpl::onConnectionError);
	connect(newRequest.data(), &ConnectRequest::fireConnectionTimeout, this, &IfdConnectorImpl::onConnectionTimeout);
	qCDebug(ifd) << "Request connection.";
	newRequest->start();
}

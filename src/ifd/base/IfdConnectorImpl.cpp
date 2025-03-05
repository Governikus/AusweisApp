/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "IfdConnectorImpl.h"

#include "Env.h"
#include "WebSocketChannel.h"

#include <QLoggingCategory>
#include <QMutableListIterator>


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
	QMutableListIterator<QSharedPointer<ConnectRequest>> requestIterator(mPendingRequests);
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


void IfdConnectorImpl::onConnectRequest(const IfdDescriptor& pIfdDescriptor, const QByteArray& pPsk)
{
	if (pIfdDescriptor.isNull() || pIfdDescriptor.getIfdName().isEmpty())
	{
		Q_EMIT fireDispatcherError(pIfdDescriptor, IfdErrorCode::INVALID_REQUEST);
		return;
	}

	if (!pIfdDescriptor.isSupported())
	{
		Q_EMIT fireDispatcherError(pIfdDescriptor, IfdErrorCode::NO_SUPPORTED_API_LEVEL);
		return;
	}

	const QSharedPointer<ConnectRequest> newRequest(new ConnectRequest(pIfdDescriptor, pPsk, mConnectTimeoutMs), &QObject::deleteLater);
	mPendingRequests += newRequest;
	connect(newRequest.data(), &ConnectRequest::fireConnectionCreated, this, &IfdConnectorImpl::onConnectionCreated);
	connect(newRequest.data(), &ConnectRequest::fireConnectionError, this, &IfdConnectorImpl::onConnectionError);
	connect(newRequest.data(), &ConnectRequest::fireConnectionTimeout, this, &IfdConnectorImpl::onConnectionTimeout);
	qCDebug(ifd) << "Request connection.";
	newRequest->start();
}

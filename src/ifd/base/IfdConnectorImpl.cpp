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


QSharedPointer<ConnectRequest> IfdConnectorImpl::removeRequest(ConnectRequest const* pRequest)
{
	QMutableListIterator<QSharedPointer<ConnectRequest>> requestIterator(mPendingRequests);
	while (requestIterator.hasNext())
	{
		const QSharedPointer<ConnectRequest> item = requestIterator.next();
		Q_ASSERT(item);

		if (item.data() == pRequest)
		{
			requestIterator.remove();
			return item;
		}
	}

	return QSharedPointer<ConnectRequest>();
}


void IfdConnectorImpl::onConnectionCreated(ConnectRequest const* pRequest, const QSharedPointer<QWebSocket>& pWebSocket)
{
	const auto& connectRequest = removeRequest(pRequest);
	if (!connectRequest)
	{
		return;
	}

	const auto& ifdDescriptor = connectRequest->getIfdDescriptor();
	const QSharedPointer<DataChannel> channel(new WebSocketChannel(pWebSocket), &QObject::deleteLater);
	const IfdVersion::Version latestSupportedVersion = IfdVersion::selectLatestSupported(ifdDescriptor.getSupportedApis());
	const QSharedPointer<IfdDispatcherClient> dispatcher(Env::create<IfdDispatcherClient*>(latestSupportedVersion, channel), &QObject::deleteLater);
	Q_EMIT fireDispatcherCreated(ifdDescriptor, dispatcher);
}


void IfdConnectorImpl::onConnectionError(ConnectRequest const* pRequest, const IfdErrorCode& pError)
{
	const auto& connectRequest = removeRequest(pRequest);
	if (!connectRequest)
	{
		return;
	}

	Q_EMIT fireDispatcherError(connectRequest->getIfdDescriptor(), pError);
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
	qCDebug(ifd) << "Request connection.";
	newRequest->start();
}

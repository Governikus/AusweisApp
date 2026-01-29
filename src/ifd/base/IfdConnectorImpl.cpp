/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

#include "IfdConnectorImpl.h"

#include "Env.h"
#include "WebSocketChannel.h"

#include <QLoggingCategory>

#include <algorithm>


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
	const auto& iter = std::find_if(mPendingRequests.begin(), mPendingRequests.end(),
			[&pRequest](const auto& pCurrentRequest){
				return pCurrentRequest == pRequest;
			});

	if (iter == mPendingRequests.end())
	{
		return QSharedPointer<ConnectRequest>();
	}

	const auto request = *iter;
	mPendingRequests.erase(iter);
	return request;
}


void IfdConnectorImpl::onConnectionCreated(ConnectRequest const* pRequest, const QSharedPointer<QWebSocket>& pWebSocket)
{
	const auto& connectRequest = removeRequest(pRequest);
	if (!connectRequest)
	{
		return;
	}

	const auto& discovery = connectRequest->getDiscovery();
	const QSharedPointer<DataChannel> channel(new WebSocketChannel(pWebSocket), &QObject::deleteLater);
	const IfdVersion::Version latestSupportedVersion = IfdVersion::selectLatestSupported(discovery.getSupportedApis());
	const QSharedPointer<IfdDispatcherClient> dispatcher(Env::create<IfdDispatcherClient*>(latestSupportedVersion, channel), &QObject::deleteLater);
	Q_EMIT fireDispatcherCreated(discovery.getIfdId(), dispatcher);
}


void IfdConnectorImpl::onConnectionError(ConnectRequest const* pRequest, const IfdErrorCode& pError)
{
	const auto& connectRequest = removeRequest(pRequest);
	if (!connectRequest)
	{
		return;
	}

	Q_EMIT fireDispatcherError(connectRequest->getDiscovery().getIfdId(), pError);
}


IfdConnectorImpl::IfdConnectorImpl(int pConnectTimeoutMs)
	: mConnectTimeoutMs(pConnectTimeoutMs)
	, mPendingRequests()
{
}


void IfdConnectorImpl::onConnectRequest(const Discovery& pDiscovery, const QByteArray& pPsk)
{
	if (pDiscovery.getIfdName().isEmpty() || pDiscovery.addressesMissing())
	{
		Q_EMIT fireDispatcherError(pDiscovery.getIfdId(), IfdErrorCode::INVALID_REQUEST);
		return;
	}

	if (!pDiscovery.isSupported())
	{
		Q_EMIT fireDispatcherError(pDiscovery.getIfdId(), IfdErrorCode::NO_SUPPORTED_API_LEVEL);
		return;
	}

	const QSharedPointer<ConnectRequest> newRequest(new ConnectRequest(pDiscovery, pPsk, mConnectTimeoutMs), &QObject::deleteLater);
	mPendingRequests += newRequest;
	connect(newRequest.data(), &ConnectRequest::fireConnectionCreated, this, &IfdConnectorImpl::onConnectionCreated);
	connect(newRequest.data(), &ConnectRequest::fireConnectionError, this, &IfdConnectorImpl::onConnectionError);
	qCDebug(ifd) << "Request connection.";
	newRequest->start();
}

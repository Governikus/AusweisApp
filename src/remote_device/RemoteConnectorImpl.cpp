/*!
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteConnectorImpl.h"

#include "Env.h"
#include "RemoteDispatcher.h"
#include "SecureStorage.h"
#include "WebSocketChannel.h"

#include <QLoggingCategory>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <QMutableListIterator>
#else
#include <QMutableVectorIterator>
#endif


Q_DECLARE_LOGGING_CATEGORY(remote_device)


namespace governikus
{
template<> RemoteConnector* createNewObject<RemoteConnector*>()
{
	return new RemoteConnectorImpl();
}


} // namespace governikus


using namespace governikus;


void RemoteConnectorImpl::removeRequest(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor)
{
	QMutableVectorIterator<QSharedPointer<ConnectRequest> > requestIterator(mPendingRequests);
	while (requestIterator.hasNext())
	{
		const QSharedPointer<ConnectRequest> item = requestIterator.next();
		Q_ASSERT(item);
		if (item.isNull())
		{
			qCCritical(remote_device) << "Unexpected null pending request";

			requestIterator.remove();
		}
		else if (item->getRemoteDeviceDescriptor() == pRemoteDeviceDescriptor)
		{
			requestIterator.remove();
		}
	}
}


void RemoteConnectorImpl::onConnectionCreated(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor,
		const QSharedPointer<QWebSocket>& pWebSocket)
{
	const QSharedPointer<DataChannel> channel(new WebSocketChannel(pWebSocket), &QObject::deleteLater);
	const IfdVersion::Version latestSupportedVersion = IfdVersion::selectLatestSupported(pRemoteDeviceDescriptor.getApiVersions());
	const QSharedPointer<RemoteDispatcherClient> dispatcher(Env::create<RemoteDispatcherClient*>(latestSupportedVersion, channel), &QObject::deleteLater);

	removeRequest(pRemoteDeviceDescriptor);

	Q_EMIT fireRemoteDispatcherCreated(pRemoteDeviceDescriptor, dispatcher);
}


void RemoteConnectorImpl::onConnectionError(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor, const RemoteErrorCode& pError)
{
	removeRequest(pRemoteDeviceDescriptor);

	Q_EMIT fireRemoteDispatcherError(pRemoteDeviceDescriptor, pError);
}


void RemoteConnectorImpl::onConnectionTimeout(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor)
{
	removeRequest(pRemoteDeviceDescriptor);

	Q_EMIT fireRemoteDispatcherError(pRemoteDeviceDescriptor, RemoteErrorCode::CONNECTION_TIMEOUT);
}


RemoteConnectorImpl::RemoteConnectorImpl(int pConnectTimeoutMs)
	: mConnectTimeoutMs(pConnectTimeoutMs)
	, mPendingRequests()
{
}


void RemoteConnectorImpl::onConnectRequest(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor, const QString& pPsk)
{
	if (pRemoteDeviceDescriptor.isNull() || pRemoteDeviceDescriptor.getIfdName().isEmpty())
	{
		Q_EMIT fireRemoteDispatcherError(pRemoteDeviceDescriptor, RemoteErrorCode::INVALID_REQUEST);
		return;
	}

	// Currently, we only support API level 1.
	if (!pRemoteDeviceDescriptor.isSupported())
	{
		Q_EMIT fireRemoteDispatcherError(pRemoteDeviceDescriptor, RemoteErrorCode::NO_SUPPORTED_API_LEVEL);
		return;
	}

	const QSharedPointer<ConnectRequest> newRequest(new ConnectRequest(pRemoteDeviceDescriptor, pPsk.toUtf8(), mConnectTimeoutMs), &QObject::deleteLater);
	mPendingRequests += newRequest;
	connect(newRequest.data(), &ConnectRequest::fireConnectionCreated, this, &RemoteConnectorImpl::onConnectionCreated);
	connect(newRequest.data(), &ConnectRequest::fireConnectionError, this, &RemoteConnectorImpl::onConnectionError);
	connect(newRequest.data(), &ConnectRequest::fireConnectionTimeout, this, &RemoteConnectorImpl::onConnectionTimeout);
	qCDebug(remote_device) << "Request connection.";
	newRequest->start();
}

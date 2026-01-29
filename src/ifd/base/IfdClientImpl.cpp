/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

#include "IfdClientImpl.h"

#include "AppSettings.h"

#include <QLoggingCategory>

#include <algorithm>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(ifd)


IfdClientImpl::IfdClientImpl()
	: IfdClient()
	, mErrorCounter()
	, mIfdConnectorThread()
	, mIfdConnector()
	, mIfdConnectorPending()
{
	bootstrapConnectorThread();
}


IfdClientImpl::~IfdClientImpl()
{
	shutdownConnectorThread();
}


void IfdClientImpl::bootstrapConnectorThread()
{
	Q_ASSERT(!mIfdConnectorThread.isRunning());
	Q_ASSERT(mIfdConnector.isNull());

	mIfdConnectorThread.setObjectName(QStringLiteral("IfdConnectorThread"));

	mIfdConnector = Env::create<IfdConnector*>();
	mIfdConnector->moveToThread(&mIfdConnectorThread);
	connect(&mIfdConnectorThread, &QThread::finished, mIfdConnector.data(), &QObject::deleteLater);

	connect(mIfdConnector.data(), &IfdConnector::fireDispatcherCreated, this, &IfdClientImpl::onDispatcherCreated);
	connect(mIfdConnector.data(), &IfdConnector::fireDispatcherError, this, &IfdClientImpl::onDispatcherError);

	mIfdConnectorThread.start();
}


void IfdClientImpl::shutdownConnectorThread()
{
	if (mIfdConnectorThread.isRunning() && !mIfdConnectorThread.isInterruptionRequested())
	{
		qCDebug(ifd) << "Shutdown IfdConnector...";
		mIfdConnectorThread.requestInterruption(); // do not try to stop AGAIN from dtor
		mIfdConnectorThread.quit();
		mIfdConnectorThread.wait(5000);
		qCDebug(ifd).noquote() << mIfdConnectorThread.objectName() << "stopped:" << !mIfdConnectorThread.isRunning();

		Q_ASSERT(!mIfdConnectorThread.isRunning());
	}
}


QSharedPointer<IfdListEntry> IfdClientImpl::mapToAndTakeConnectorPending(const QByteArray& pIfdId)
{
	const auto& iter = std::find_if(mIfdConnectorPending.begin(), mIfdConnectorPending.end(),
			[&pIfdId](const auto& pEntry){
				return pEntry->containsIfdId(pIfdId);
			});

	if (iter == mIfdConnectorPending.end())
	{
		Q_ASSERT(false);
		return QSharedPointer<IfdListEntry>();
	}

	const auto entry = *iter;
	mIfdConnectorPending.erase(iter);
	return entry;
}


void IfdClientImpl::onDispatcherCreated(const QByteArray& pIfdId, const QSharedPointer<IfdDispatcherClient>& pDispatcher)
{
	mErrorCounter[pIfdId] = 0;
	const QSharedPointer<IfdListEntry>& entry = mapToAndTakeConnectorPending(pIfdId);
	if (entry.isNull())
	{
		qCDebug(ifd) << "Failed to map IfdConnector response:" << pIfdId;
		Q_ASSERT(false);
		return;
	}

	mConnectedDeviceIds.append(entry->getDiscovery().getIfdId());
	connect(pDispatcher.data(), &IfdDispatcherClient::fireClosed, this, &IfdClientImpl::onDispatcherDestroyed);

	Q_EMIT fireEstablishConnectionDone(entry, GlobalStatus::Code::No_Error);
	Q_EMIT fireNewDispatcher(pDispatcher);
}


void IfdClientImpl::onDispatcherError(const QByteArray& pIfdId, IfdErrorCode pErrorCode)
{
	const QSharedPointer<IfdListEntry>& entry = mapToAndTakeConnectorPending(pIfdId);
	if (entry.isNull())
	{
		qCDebug(ifd) << "Failed to map IfdConnector response:" << pIfdId;
		Q_ASSERT(false);
		return;
	}

	if (pErrorCode == IfdErrorCode::REMOTE_HOST_REFUSED_CONNECTION || pErrorCode == IfdErrorCode::NO_SUPPORTED_API_LEVEL)
	{
		mErrorCounter[pIfdId] += 1;
		if (mErrorCounter.value(pIfdId) >= 7)
		{
			qCCritical(ifd) << "Remote device refused connection seven times, removing certificate with fingerprint:" << pIfdId;
			RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
			const QString& deviceNameEscaped = settings.getRemoteInfo(pIfdId).getNameEscaped();
			settings.removeTrustedCertificate(pIfdId);
			mErrorCounter[pIfdId] = 0;
			if (!deviceNameEscaped.isEmpty())
			{
				Q_EMIT fireCertificateRemoved(deviceNameEscaped);
			}
		}
	}
	else
	{
		mErrorCounter[pIfdId] = 0;
	}

	Q_EMIT fireEstablishConnectionDone(entry, IfdConnector::errorToGlobalStatus(pErrorCode));
}


void IfdClientImpl::establishConnection(const QSharedPointer<IfdListEntry>& pEntry, const QByteArray& pPsk)
{
	if (mIfdConnector.isNull())
	{
		qCCritical(ifd) << "There is no IfdConnector!";
		Q_ASSERT(false);
	}

	if (!pEntry)
	{
		qCWarning(ifd) << "Cannot connect to given device.";
		Q_EMIT fireEstablishConnectionDone(pEntry, GlobalStatus::Code::IfdConnector_ConnectionError);
		return;
	}

	mIfdConnectorPending += pEntry;

	qCDebug(ifd) << "Establishing connection to remote device.";
	const auto& localCopy = mIfdConnector;
	QMetaObject::invokeMethod(localCopy.data(), [localCopy, pEntry, pPsk] {
				localCopy->onConnectRequest(pEntry->getDiscovery(), pPsk);
			}, Qt::QueuedConnection);
}


QByteArrayList IfdClientImpl::getConnectedDeviceIDs() const
{
	return mConnectedDeviceIds;
}


void IfdClientImpl::onDispatcherDestroyed(GlobalStatus::Code pCloseCode, const QByteArray& pId)
{
	mConnectedDeviceIds.removeAll(pId);
	Q_EMIT fireDispatcherDestroyed(pCloseCode, pId);
}

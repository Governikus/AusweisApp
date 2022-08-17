/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#include "IfdClientImpl.h"

#include "AppSettings.h"
#include "IfdConnectorImpl.h"

#include <QLoggingCategory>

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


QSharedPointer<IfdListEntry> IfdClientImpl::mapToAndTakeConnectorPending(const IfdDescriptor& pIfdDescriptor)
{
	QMutableVectorIterator<QSharedPointer<IfdListEntry>> i(mIfdConnectorPending);
	while (i.hasNext())
	{
		QSharedPointer<IfdListEntry> entry = i.next();
		if (entry->containsEquivalent(pIfdDescriptor))
		{
			i.remove();
			return entry;
		}
	}

	Q_ASSERT(false);
	return QSharedPointer<IfdListEntry>();
}


void IfdClientImpl::onDispatcherCreated(const IfdDescriptor& pIfdDescriptor, const QSharedPointer<IfdDispatcherClient>& pDispatcher)
{
	mErrorCounter[pIfdDescriptor.getIfdId()] = 0;
	const QSharedPointer<IfdListEntry>& entry = mapToAndTakeConnectorPending(pIfdDescriptor);
	if (entry.isNull())
	{
		qCDebug(ifd) << "Failed to map IfdConnector response:" << pIfdDescriptor;
		Q_ASSERT(false);
		return;
	}

	mConnectedDeviceIds.append(entry->getIfdDescriptor().getIfdId());
	connect(pDispatcher.data(), &IfdDispatcherClient::fireClosed, this, &IfdClientImpl::onDispatcherDestroyed);

	Q_EMIT fireEstablishConnectionDone(entry, GlobalStatus::Code::No_Error);
	Q_EMIT fireNewDispatcher(pDispatcher);
}


void IfdClientImpl::onDispatcherError(const IfdDescriptor& pIfdDescriptor, IfdErrorCode pErrorCode)
{
	const QSharedPointer<IfdListEntry>& entry = mapToAndTakeConnectorPending(pIfdDescriptor);
	if (entry.isNull())
	{
		qCDebug(ifd) << "Failed to map IfdConnector response:" << pIfdDescriptor;
		Q_ASSERT(false);
		return;
	}

	const auto& ifdId = pIfdDescriptor.getIfdId();
	if (pErrorCode == IfdErrorCode::REMOTE_HOST_REFUSED_CONNECTION || pErrorCode == IfdErrorCode::NO_SUPPORTED_API_LEVEL)
	{
		mErrorCounter[ifdId] += 1;
		if (mErrorCounter.value(ifdId) >= 7)
		{
			qCCritical(ifd) << "Remote device refused connection seven times, removing certificate with fingerprint:" << ifdId;
			RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
			const QString& deviceNameEscaped = settings.getRemoteInfo(ifdId).getNameEscaped();
			settings.removeTrustedCertificate(ifdId);
			mErrorCounter[ifdId] = 0;
			if (!deviceNameEscaped.isEmpty())
			{
				Q_EMIT fireCertificateRemoved(deviceNameEscaped);
			}
		}
	}
	else
	{
		mErrorCounter[ifdId] = 0;
	}

	Q_EMIT fireEstablishConnectionDone(entry, IfdConnector::errorToGlobalStatus(pErrorCode));
}


void IfdClientImpl::establishConnection(const QSharedPointer<IfdListEntry>& pEntry, const QString& pPsk)
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
			localCopy->onConnectRequest(pEntry->getIfdDescriptor(), pPsk);
		}, Qt::QueuedConnection);
}


QStringList IfdClientImpl::getConnectedDeviceIDs() const
{
	return mConnectedDeviceIds;
}


void IfdClientImpl::onDispatcherDestroyed(GlobalStatus::Code pCloseCode, const QString& pId)
{
	mConnectedDeviceIds.removeAll(pId);
	Q_EMIT fireDispatcherDestroyed(pCloseCode, pId);
}

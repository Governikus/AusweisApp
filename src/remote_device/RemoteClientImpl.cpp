/*!
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteClientImpl.h"

#include "AppSettings.h"
#include "messages/Discovery.h"
#include "RemoteConnectorImpl.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(remote_device)

using namespace governikus;

namespace governikus
{
template<> RemoteClient* createNewObject<RemoteClient*>()
{
	return new RemoteClientImpl();
}


} // namespace governikus


RemoteClientImpl::RemoteClientImpl()
	: mDatagramHandler()
	, mRemoteDeviceList(Env::create<RemoteDeviceList*>())
	, mErrorCounter()
	, mRemoteConnectorThread()
	, mRemoteConnector()
	, mRemoteConnectorPending()
{
	connect(mRemoteDeviceList.data(), &RemoteDeviceList::fireDeviceAppeared, this, &RemoteClient::fireDeviceAppeared);
	connect(mRemoteDeviceList.data(), &RemoteDeviceList::fireDeviceUpdated, this, &RemoteClient::fireDeviceUpdated);
	connect(mRemoteDeviceList.data(), &RemoteDeviceList::fireDeviceVanished, this, &RemoteClient::fireDeviceVanished);

	bootstrapRemoteConnectorThread();
}


RemoteClientImpl::~RemoteClientImpl()
{
	shutdownRemoteConnectorThread();
}


void RemoteClientImpl::bootstrapRemoteConnectorThread()
{
	Q_ASSERT(!mRemoteConnectorThread.isRunning());
	Q_ASSERT(mRemoteConnector.isNull());

	mRemoteConnectorThread.setObjectName(QStringLiteral("RemoteConnectorThread"));

	mRemoteConnector = Env::create<RemoteConnector*>();
	mRemoteConnector->moveToThread(&mRemoteConnectorThread);
	connect(&mRemoteConnectorThread, &QThread::finished, mRemoteConnector.data(), &QObject::deleteLater);

	connect(mRemoteConnector.data(), &RemoteConnector::fireRemoteDispatcherCreated, this, &RemoteClientImpl::onRemoteDispatcherCreated);
	connect(mRemoteConnector.data(), &RemoteConnector::fireRemoteDispatcherError, this, &RemoteClientImpl::onRemoteDispatcherError);

	mRemoteConnectorThread.start();
}


void RemoteClientImpl::shutdownRemoteConnectorThread()
{
	if (mRemoteConnectorThread.isRunning() && !mRemoteConnectorThread.isInterruptionRequested())
	{
		qCDebug(remote_device) << "Shutdown RemoteConnector...";
		mRemoteConnectorThread.requestInterruption(); // do not try to stop AGAIN from dtor
		mRemoteConnectorThread.quit();
		mRemoteConnectorThread.wait(5000);
		qCDebug(remote_device).noquote() << mRemoteConnectorThread.objectName() << "stopped:" << !mRemoteConnectorThread.isRunning();

		Q_ASSERT(!mRemoteConnectorThread.isRunning());
	}
}


QSharedPointer<RemoteDeviceListEntry> RemoteClientImpl::mapToAndTakeRemoteConnectorPending(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor)
{
	QMutableVectorIterator<QSharedPointer<RemoteDeviceListEntry> > i(mRemoteConnectorPending);
	while (i.hasNext())
	{
		QSharedPointer<RemoteDeviceListEntry> entry = i.next();
		if (entry->containsEquivalent(pRemoteDeviceDescriptor))
		{
			i.remove();
			return entry;
		}
	}

	Q_ASSERT(false);
	return QSharedPointer<RemoteDeviceListEntry>();
}


void RemoteClientImpl::onNewMessage(const QByteArray& pData, const QHostAddress& pAddress)
{
	QJsonObject obj;
	{
		obj = RemoteMessage::parseByteArray(pData);
	}

	const Discovery discovery(obj);
	if (discovery.isIncomplete())
	{
		qCDebug(remote_device) << "Discarding unparsable message";
		return;
	}

	const RemoteDeviceDescriptor remoteDeviceDescriptor(discovery, pAddress);
	if (remoteDeviceDescriptor.isNull())
	{
		qCCritical(remote_device) << "Dropping message from unparsable address:" << pAddress;
		return;
	}

	mRemoteDeviceList->update(remoteDeviceDescriptor);
}


void RemoteClientImpl::onRemoteDispatcherCreated(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor, const QSharedPointer<RemoteDispatcherClient>& pDispatcher)
{
	mErrorCounter[pRemoteDeviceDescriptor.getIfdId()] = 0;
	const QSharedPointer<RemoteDeviceListEntry>& entry = mapToAndTakeRemoteConnectorPending(pRemoteDeviceDescriptor);
	if (entry.isNull())
	{
		qCDebug(remote_device) << "Failed to map RemoteConnector response:" << pRemoteDeviceDescriptor;
		Q_ASSERT(false);
		return;
	}

	mConnectedDeviceIds.append(entry->getRemoteDeviceDescriptor().getIfdId());
	connect(pDispatcher.data(), &RemoteDispatcherClient::fireClosed, this, &RemoteClientImpl::onDispatcherDestroyed);

	Q_EMIT fireEstablishConnectionDone(entry, GlobalStatus::Code::No_Error);
	Q_EMIT fireNewRemoteDispatcher(pDispatcher);
}


void RemoteClientImpl::onRemoteDispatcherError(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor, RemoteErrorCode pErrorCode)
{
	const QSharedPointer<RemoteDeviceListEntry>& entry = mapToAndTakeRemoteConnectorPending(pRemoteDeviceDescriptor);
	if (entry.isNull())
	{
		qCDebug(remote_device) << "Failed to map RemoteConnector response:" << pRemoteDeviceDescriptor;
		Q_ASSERT(false);
		return;
	}

	if (pErrorCode == RemoteErrorCode::REMOTE_HOST_REFUSED_CONNECTION || pErrorCode == RemoteErrorCode::NO_SUPPORTED_API_LEVEL)
	{
		const auto ifd = pRemoteDeviceDescriptor.getIfdId();
		mErrorCounter[ifd] += 1;
		if (mErrorCounter.value(ifd) >= 7)
		{
			qCCritical(remote_device) << "Remote device refused connection seven times, removing certificate with fingerprint:" << pRemoteDeviceDescriptor.getIfdId();
			RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
			QString deviceNameEscaped;
			const auto& infos = settings.getRemoteInfos();
			for (const auto& remoteInfo : infos)
			{
				if (remoteInfo.getFingerprint() == pRemoteDeviceDescriptor.getIfdId())
				{
					deviceNameEscaped = remoteInfo.getNameEscaped();
				}
			}
			settings.removeTrustedCertificate(pRemoteDeviceDescriptor.getIfdId());
			mErrorCounter[pRemoteDeviceDescriptor.getIfdId()] = 0;
			if (!deviceNameEscaped.isEmpty())
			{
				Q_EMIT fireCertificateRemoved(deviceNameEscaped);
			}
		}
	}
	else
	{
		mErrorCounter[pRemoteDeviceDescriptor.getIfdId()] = 0;
	}

	Q_EMIT fireEstablishConnectionDone(entry, RemoteConnector::errorToGlobalStatus(pErrorCode));
}


void RemoteClientImpl::startDetection()
{
	if (!mDatagramHandler.isNull())
	{
		qCDebug(remote_device) << "Detection already started";
		return;
	}

	mDatagramHandler.reset(Env::create<DatagramHandler*>());
	connect(mDatagramHandler.data(), &DatagramHandler::fireNewMessage, this, &RemoteClientImpl::onNewMessage);
	Q_EMIT fireDetectionChanged();
}


void RemoteClientImpl::stopDetection()
{
	mDatagramHandler.reset();
	mRemoteDeviceList->clear();
	Q_EMIT fireDetectionChanged();
}


bool RemoteClientImpl::isDetecting()
{
	return !mDatagramHandler.isNull();
}


void RemoteClientImpl::establishConnection(const QSharedPointer<RemoteDeviceListEntry>& pEntry, const QString& pPsk)
{
	if (mRemoteConnector.isNull())
	{
		qCCritical(remote_device) << "There is no RemoteConnector!";
		Q_ASSERT(false);
	}

	if (!pEntry)
	{
		qCWarning(remote_device) << "Cannot connect to given device.";
		Q_EMIT fireEstablishConnectionDone(pEntry, GlobalStatus::Code::RemoteConnector_ConnectionError);
		return;
	}

	mRemoteConnectorPending += pEntry;

	qCDebug(remote_device) << "Establishing connection to remote device.";
	const auto& localCopy = mRemoteConnector;
	QMetaObject::invokeMethod(localCopy.data(), [localCopy, pEntry, pPsk] {
				localCopy->onConnectRequest(pEntry->getRemoteDeviceDescriptor(), pPsk);
			}, Qt::QueuedConnection);
}


QVector<QSharedPointer<RemoteDeviceListEntry> > RemoteClientImpl::getAnnouncingRemoteDevices() const
{
	return mRemoteDeviceList->getRemoteDevices();
}


void RemoteClientImpl::requestRemoteDevices()
{
	Q_EMIT fireRemoteDevicesInfo(mRemoteDeviceList->getRemoteDevices());
}


QStringList RemoteClientImpl::getConnectedDeviceIDs() const
{
	return mConnectedDeviceIds;
}


QVector<RemoteServiceSettings::RemoteInfo> RemoteClientImpl::getConnectedDeviceInfos()
{
	const RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	auto remoteInfos = settings.getRemoteInfos();
	QVector<RemoteServiceSettings::RemoteInfo> result;
	for (const auto& info : remoteInfos)
	{
		for (const auto& id : qAsConst(mConnectedDeviceIds))
		{
			if (info.getFingerprint() == id)
			{
				result.append(info);
			}
		}
	}
	return result;
}


void RemoteClientImpl::onDispatcherDestroyed(GlobalStatus::Code pCloseCode, const QString& pId)
{
	mConnectedDeviceIds.removeAll(pId);
	Q_EMIT fireDispatcherDestroyed(pCloseCode, pId);
}

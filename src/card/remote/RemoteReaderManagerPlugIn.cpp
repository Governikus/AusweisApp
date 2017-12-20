/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteReaderManagerPlugIn.h"

#include "AppSettings.h"
#include "FuncUtils.h"
#include "messages/GetIfdStatus.h"
#include "messages/IfdError.h"
#include "messages/IfdEstablishContext.h"
#include "messages/IfdEstablishContextResponse.h"
#include "messages/IfdStatus.h"
#include "RemoteClient.h"
#include "RemoteDeviceList.h"
#include "RemoteReader.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_remote)


void RemoteReaderManagerPlugIn::updateReader(const IfdStatus& pIfdStatus)
{
	const QSharedPointer<RemoteDispatcher> remoteToUpdate = mRemoteToUpdate.toStrongRef();
	const QString& contextHandle = remoteToUpdate->getContextHandle();
	const QString& readerName = pIfdStatus.getSlotName() + contextHandle;

	if (mReaderList.contains(readerName))
	{
		if (pIfdStatus.getConnectedReader())
		{
			static_cast<RemoteReader*>(mReaderList[readerName])->update(pIfdStatus);
		}
		else
		{
			qCDebug(card_remote) << "Removed reader" << readerName;
			Q_EMIT fireReaderRemoved(readerName);
			delete mReaderList.take(readerName);
			mRemoteDispatchers.remove(remoteToUpdate, readerName);
		}
		return;
	}

	if (pIfdStatus.getConnectedReader())
	{
		RemoteReader* reader = new RemoteReader(readerName, remoteToUpdate, pIfdStatus);

		connect(reader, &RemoteReader::fireCardInserted, this, &RemoteReaderManagerPlugIn::fireCardInserted);
		connect(reader, &RemoteReader::fireCardRemoved, this, &RemoteReaderManagerPlugIn::fireCardRemoved);
		connect(reader, &RemoteReader::fireCardRetryCounterChanged, this, &RemoteReaderManagerPlugIn::fireCardRetryCounterChanged);
		connect(reader, &RemoteReader::fireReaderPropertiesUpdated, this, &RemoteReaderManagerPlugIn::fireReaderPropertiesUpdated);

		mReaderList.insert(readerName, reader);
		mRemoteDispatchers.insert(remoteToUpdate, readerName);
		qCDebug(card_remote) << "Add reader" << readerName;
		Q_EMIT fireReaderAdded(readerName);

		// Also update card
		reader->update(pIfdStatus);
	}
}


void RemoteReaderManagerPlugIn::removeDispatcher(const QSharedPointer<RemoteDispatcher>& pRemoteDispatcher)
{
	const auto& remoteReader = mRemoteDispatchers.values(pRemoteDispatcher);
	for (const auto& readerName : remoteReader)
	{
		if (readerName.isEmpty())
		{
			continue;
		}
		Q_EMIT fireReaderRemoved(readerName);
		delete mReaderList.take(readerName);
	}
	disconnect(pRemoteDispatcher.data(), &RemoteDispatcher::fireReceived, this, &RemoteReaderManagerPlugIn::onRemoteMessage);
	disconnect(pRemoteDispatcher.data(), &RemoteDispatcher::fireClosed, this, &RemoteReaderManagerPlugIn::onDispatcherClosed);

	mRemoteDispatchers.remove(pRemoteDispatcher);
}


void RemoteReaderManagerPlugIn::removeAllDispatchers()
{
	const auto& keys = mRemoteDispatchers.keys();
	for (const auto& dispatcher : keys)
	{
		removeDispatcher(dispatcher);
	}
}


void RemoteReaderManagerPlugIn::connectToPairedReaders()
{
	if (!mRemoteClient.isNull())
	{
		connect(mRemoteClient.data(), &RemoteClient::fireRemoteDevicesInfo, this, &RemoteReaderManagerPlugIn::continueConnectToPairedReaders);
		QMetaObject::invokeMethod(mRemoteClient.data(), "requestRemoteDevices", Qt::QueuedConnection);
	}
}


void RemoteReaderManagerPlugIn::unexpectedMessage(const QSharedPointer<const RemoteMessage>& pMessage, const QSharedPointer<RemoteDispatcher>& pRemoteDispatcher, bool pSendMessage)
{
	qCWarning(card_remote) << "Received an unexpected message of type:" << pMessage->getType();

	if (pSendMessage)
	{
		const QSharedPointer<const IfdError> errorMessage(new IfdError(QString(), QStringLiteral("/al/common#unknownAPIFunction")));
		QMetaObject::invokeMethod(pRemoteDispatcher.data(), "send", Qt::QueuedConnection, Q_ARG(QSharedPointer<const RemoteMessage>, errorMessage));
	}
}


void RemoteReaderManagerPlugIn::continueConnectToPairedReaders(const QVector<QSharedPointer<RemoteDeviceListEntry> >& pRemoteDevices)
{
	disconnect(mRemoteClient.data(), &RemoteClient::fireRemoteDevicesInfo, this, &RemoteReaderManagerPlugIn::continueConnectToPairedReaders);

	const QList<QSharedPointer<RemoteDispatcher> > remoteDispatchers = mRemoteDispatchers.keys();
	const QStringList connectionIds = map<QSharedPointer<RemoteDispatcher>, QString>([](const QSharedPointer<RemoteDispatcher>& d){
				return d->getId();
			},
			remoteDispatchers);

	const RemoteServiceSettings& remoteServiceSettings = AppSettings::getInstance().getRemoteServiceSettings();
	for (const QSharedPointer<RemoteDeviceListEntry>& remoteDevice : pRemoteDevices)
	{
		const QString ifdId = remoteDevice->getRemoteDeviceDescriptor().getIfdId();

		// If already connected: skip.
		if (connectionIds.contains(ifdId))
		{
			continue;
		}

		const RemoteServiceSettings::RemoteInfo remoteInfo = remoteServiceSettings.getRemoteInfo(ifdId);
		// If we find a remote info for this fingerprint (IfdId), then the remote device is paired.
		if (remoteInfo.getFingerprint() == ifdId)
		{
			QMetaObject::invokeMethod(mRemoteClient.data(), "establishConnection", Qt::QueuedConnection, Q_ARG(QSharedPointer<RemoteDeviceListEntry>, remoteDevice), Q_ARG(QString, QString()));
		}
	}
}


void RemoteReaderManagerPlugIn::onRemoteMessage(const QSharedPointer<const RemoteMessage>& pMessage, const QSharedPointer<RemoteDispatcher>& pRemoteDispatcher)
{
	const QVector<RemoteCardMessageType> clientMessageTypes({
				RemoteCardMessageType::IFDEstablishContext,
				RemoteCardMessageType::IFDGetStatus,
				RemoteCardMessageType::IFDConnect,
				RemoteCardMessageType::IFDDisconnect,
				RemoteCardMessageType::IFDTransmit,
				RemoteCardMessageType::IFDEstablishPACEChannel
			});

	if (clientMessageTypes.contains(pMessage->getType()))
	{
		unexpectedMessage(pMessage, pRemoteDispatcher, true);
		return;
	}

	mRemoteToUpdate = pRemoteDispatcher;
	receive(pMessage, pRemoteDispatcher);
}


void RemoteReaderManagerPlugIn::onDispatcherClosed(GlobalStatus::Code pCloseCode, const QSharedPointer<RemoteDispatcher>& pRemoteDispatcher)
{
	qCDebug(card_remote) << "RemoteDispatcher was closed with:" << pCloseCode;
	removeDispatcher(pRemoteDispatcher);
}


void RemoteReaderManagerPlugIn::checkRemoteDevices()
{
	if (mConnectToKnownReaders)
	{
		connectToPairedReaders();
	}
}


void RemoteReaderManagerPlugIn::onConnectToKnownReadersChanged()
{
	if (mConnectToKnownReaders)
	{
		connectToPairedReaders();
	}
	else
	{
		removeAllDispatchers();
	}
}


RemoteReaderManagerPlugIn::RemoteReaderManagerPlugIn()
	: ReaderManagerPlugIn(ReaderManagerPlugInType::REMOTE, true)
	, MessageReceiver()
	, mScanTimer()
	, mRemoteClient()
	, mReaderList()
{
	mScanTimer.setInterval(1000);
	connect(&mScanTimer, &QTimer::timeout, this, &RemoteReaderManagerPlugIn::checkRemoteDevices);
}


RemoteReaderManagerPlugIn::~RemoteReaderManagerPlugIn()
{
	mScanTimer.stop();
	removeAllDispatchers();
}


void RemoteReaderManagerPlugIn::setRemoteClient(const QSharedPointer<RemoteClient>& pRemoteClient)
{
	mRemoteClient = pRemoteClient;
	if (!mRemoteClient.isNull())
	{
		connect(mRemoteClient.data(), &RemoteClient::fireNewRemoteDispatcher, this, &RemoteReaderManagerPlugIn::addRemoteDispatcher);
	}
}


QList<Reader*> RemoteReaderManagerPlugIn::getReaders() const
{
	return mReaderList.values();
}


void RemoteReaderManagerPlugIn::addRemoteDispatcher(const QSharedPointer<RemoteDispatcher>& pRemoteDispatcher)
{
	Q_ASSERT(pRemoteDispatcher);

	mRemoteDispatchers.insert(pRemoteDispatcher, QString());

	connect(pRemoteDispatcher.data(), &RemoteDispatcher::fireReceived, this, &RemoteReaderManagerPlugIn::onRemoteMessage);
	connect(pRemoteDispatcher.data(), &RemoteDispatcher::fireClosed, this, &RemoteReaderManagerPlugIn::onDispatcherClosed);

	const QSharedPointer<const IfdEstablishContext> establishContext(new IfdEstablishContext(QStringLiteral("IFDInterface_WebSocket_v0"), DeviceInfo::getName()));
	QMetaObject::invokeMethod(pRemoteDispatcher.data(), "send", Qt::QueuedConnection, Q_ARG(QSharedPointer<const RemoteMessage>, establishContext));
}


void RemoteReaderManagerPlugIn::process(const QSharedPointer<const IfdEstablishContextResponse>& pMessage)
{
	if (pMessage->resultHasError())
	{
		qCDebug(card_remote) << "EstablishContext failed:" << pMessage->getResultMinor();
		return;
	}

	bool initialPairing = false;

	//update IFD name
	if (mRemoteToUpdate)
	{
		RemoteServiceSettings& settings = AppSettings::getInstance().getRemoteServiceSettings();
		auto info = settings.getRemoteInfo(mRemoteToUpdate.data()->getId());
		if (info.getName().isEmpty())
		{
			initialPairing = true;
		}
		info.setName(pMessage->getIfdName());
		settings.updateRemoteInfo(info);
	}

	if (initialPairing)
	{
		removeDispatcher(mRemoteToUpdate);
	}
	else
	{
		const QSharedPointer<const GetIfdStatus> getIfdStatus(new GetIfdStatus());
		QMetaObject::invokeMethod(mRemoteToUpdate.data(), "send", Qt::QueuedConnection, Q_ARG(QSharedPointer<const RemoteMessage>, getIfdStatus));
	}
}


void RemoteReaderManagerPlugIn::process(const QSharedPointer<const IfdStatus>& pMessage)
{
	updateReader(*pMessage);
}


void RemoteReaderManagerPlugIn::unprocessed(const QSharedPointer<const RemoteMessage>& pMessage)
{
	unexpectedMessage(pMessage);
}


void RemoteReaderManagerPlugIn::startScan()
{
	if (mRemoteClient.isNull())
	{
		qCWarning(card_remote) << "Cannot start remote device discovery: no remote client found";
		return;
	}

	connect(mRemoteClient.data(), &RemoteClient::fireDeviceAppeared, this, &RemoteReaderManagerPlugIn::checkRemoteDevices);

	mScanTimer.start();
	QMetaObject::invokeMethod(mRemoteClient.data(), "startDetection", Qt::QueuedConnection);
	mScanInProgress = true;
}


void RemoteReaderManagerPlugIn::stopScan()
{
	if (mRemoteClient.isNull())
	{
		qCWarning(card_remote) << "Cannot stop remote device discovery: no remote client found";
		return;
	}

	disconnect(mRemoteClient.data(), &RemoteClient::fireDeviceAppeared, this, &RemoteReaderManagerPlugIn::checkRemoteDevices);
	mScanTimer.stop();
	QMetaObject::invokeMethod(mRemoteClient.data(), "stopDetection", Qt::QueuedConnection);
	mScanInProgress = false;
}

/*!
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteReaderManagerPlugIn.h"

#include "AppSettings.h"
#include "messages/IfdError.h"
#include "messages/IfdEstablishContext.h"
#include "messages/IfdEstablishContextResponse.h"
#include "messages/IfdGetStatus.h"
#include "messages/IfdStatus.h"
#include "RemoteClient.h"
#include "RemoteDeviceList.h"
#include "RemoteReader.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_remote)


void RemoteReaderManagerPlugIn::removeDispatcher(const QString& pId)
{
	const auto& remoteReader = mReadersForDispatcher.values(pId);
	for (const auto& readerName : remoteReader)
	{
		if (readerName.isEmpty())
		{
			continue;
		}
		Q_EMIT fireReaderRemoved(readerName);
		delete mReaderList.take(readerName);
		mReadersForDispatcher.remove(pId, readerName);
	}

	const auto remoteDispatcher = mDispatcherList[pId].data();
	disconnect(remoteDispatcher, &RemoteDispatcherClient::fireContextEstablished, this, &RemoteReaderManagerPlugIn::onContextEstablished);
	disconnect(remoteDispatcher, &RemoteDispatcherClient::fireReceived, this, &RemoteReaderManagerPlugIn::onRemoteMessage);
	disconnect(remoteDispatcher, &RemoteDispatcherClient::fireClosed, this, &RemoteReaderManagerPlugIn::onDispatcherClosed);

	mDispatcherList.remove(pId);
}


void RemoteReaderManagerPlugIn::removeAllDispatchers()
{
	for (const auto& dispatcher : qAsConst(mDispatcherList))
	{
		QMetaObject::invokeMethod(dispatcher.data(), &RemoteDispatcherClient::close, Qt::QueuedConnection);
	}
}


void RemoteReaderManagerPlugIn::connectToPairedReaders()
{
	if (!mConnectToPairedReaders || mConnectionCheckInProgress)
	{
		return;
	}

	mConnectionCheckInProgress = true;
	const auto remoteClient = Env::getSingleton<RemoteClient>();
	connect(remoteClient, &RemoteClient::fireRemoteDevicesInfo, this, &RemoteReaderManagerPlugIn::continueConnectToPairedReaders);
	QMetaObject::invokeMethod(remoteClient, &RemoteClient::requestRemoteDevices, Qt::QueuedConnection);
}


void RemoteReaderManagerPlugIn::handleIFDStatus(const QJsonObject& pJsonObject, const QString& pId)
{
	const auto it = mDispatcherList.find(pId);
	if (it == mDispatcherList.end())
	{
		return;
	}
	const auto& remoteDispatcher = *it;
	IfdStatus ifdStatus(pJsonObject);

	const QString& contextHandle = remoteDispatcher->getContextHandle();
	const QString& readerName = ifdStatus.getSlotName() + contextHandle;

	if (mReaderList.contains(readerName))
	{
		if (ifdStatus.getConnectedReader())
		{
			static_cast<RemoteReader*>(mReaderList[readerName])->update(ifdStatus);
		}
		else
		{
			qCDebug(card_remote) << "Removed reader" << readerName;
			Q_EMIT fireReaderRemoved(readerName);
			delete mReaderList.take(readerName);
			mReadersForDispatcher.remove(pId, readerName);
		}
		return;
	}

	if (ifdStatus.getConnectedReader())
	{
		auto reader = new RemoteReader(readerName, remoteDispatcher, ifdStatus);

		connect(reader, &RemoteReader::fireCardInserted, this, &RemoteReaderManagerPlugIn::fireCardInserted);
		connect(reader, &RemoteReader::fireCardRemoved, this, &RemoteReaderManagerPlugIn::fireCardRemoved);
		connect(reader, &RemoteReader::fireCardRetryCounterChanged, this, &RemoteReaderManagerPlugIn::fireCardRetryCounterChanged);
		connect(reader, &RemoteReader::fireReaderPropertiesUpdated, this, &RemoteReaderManagerPlugIn::fireReaderPropertiesUpdated);

		mReaderList.insert(readerName, reader);
		mReadersForDispatcher.insert(pId, readerName);
		qCDebug(card_remote) << "Add reader" << readerName;
		Q_EMIT fireReaderAdded(readerName);

		// Also update card
		reader->update(ifdStatus);
	}
}


void RemoteReaderManagerPlugIn::continueConnectToPairedReaders(const QVector<QSharedPointer<RemoteDeviceListEntry> >& pRemoteDevices)
{
	const auto remoteClient = Env::getSingleton<RemoteClient>();

	disconnect(remoteClient, &RemoteClient::fireRemoteDevicesInfo, this, &RemoteReaderManagerPlugIn::continueConnectToPairedReaders);

	const RemoteServiceSettings& remoteServiceSettings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	for (const QSharedPointer<RemoteDeviceListEntry>& remoteDevice : pRemoteDevices)
	{
		if (!remoteDevice->getRemoteDeviceDescriptor().isSupported())
		{
			continue;
		}

		const QString ifdId = remoteDevice->getRemoteDeviceDescriptor().getIfdId();

		// If already connected: skip.
		if (mDispatcherList.contains(ifdId))
		{
			continue;
		}

		const RemoteServiceSettings::RemoteInfo remoteInfo = remoteServiceSettings.getRemoteInfo(ifdId);
		// If we find a remote info for this fingerprint (IfdId), then the remote device is paired.
		if (remoteInfo.getFingerprint() == ifdId)
		{
			QMetaObject::invokeMethod(remoteClient, [remoteClient, remoteDevice] {
						remoteClient->establishConnection(remoteDevice, QString());
					}, Qt::QueuedConnection);
		}
	}
	mConnectionCheckInProgress = false;
}


void RemoteReaderManagerPlugIn::onContextEstablished(const QString& pIfdName, const QString& pId)
{
	RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	auto info = settings.getRemoteInfo(pId);
	bool initialPairing = false;
	if (info.getNameEscaped().isEmpty())
	{
		initialPairing = true;
	}
	info.setNameUnescaped(pIfdName);
	settings.updateRemoteInfo(info);

	const auto& remoteDispatcher = mDispatcherList[pId];
	if (initialPairing)
	{
		QMetaObject::invokeMethod(remoteDispatcher.data(), &RemoteDispatcherClient::close, Qt::QueuedConnection);
	}
	else
	{
		QMetaObject::invokeMethod(remoteDispatcher.data(), [remoteDispatcher] {
					const QSharedPointer<const IfdGetStatus>& ifdGetStatus = QSharedPointer<IfdGetStatus>::create();
					remoteDispatcher->send(ifdGetStatus);
				}, Qt::QueuedConnection);
	}
}


void RemoteReaderManagerPlugIn::onRemoteMessage(RemoteCardMessageType pMessageType, const QJsonObject& pJsonObject, const QString& pId)
{
	switch (pMessageType)
	{
		case RemoteCardMessageType::IFDError:
		case RemoteCardMessageType::UNDEFINED:
		case RemoteCardMessageType::IFDEstablishContextResponse:
		case RemoteCardMessageType::IFDConnectResponse:
		case RemoteCardMessageType::IFDTransmitResponse:
		case RemoteCardMessageType::IFDDisconnectResponse:
		case RemoteCardMessageType::IFDEstablishPACEChannelResponse:
		case RemoteCardMessageType::IFDModifyPINResponse:
			break;

		case RemoteCardMessageType::IFDEstablishContext:
		case RemoteCardMessageType::IFDGetStatus:
		case RemoteCardMessageType::IFDConnect:
		case RemoteCardMessageType::IFDDisconnect:
		case RemoteCardMessageType::IFDTransmit:
		case RemoteCardMessageType::IFDEstablishPACEChannel:
		case RemoteCardMessageType::IFDModifyPIN:
		{
			qCWarning(card_remote) << "Received an unexpected message of type:" << pMessageType;
			const auto& dispatcher = mDispatcherList[pId];
			QMetaObject::invokeMethod(dispatcher.data(), [dispatcher] {
						const QSharedPointer<const IfdError>& errorMessage = QSharedPointer<IfdError>::create(QString(), ECardApiResult::Minor::AL_Unkown_API_Function);
						dispatcher->send(errorMessage);
					}, Qt::QueuedConnection);
			break;
		}

		case RemoteCardMessageType::IFDStatus:
			handleIFDStatus(pJsonObject, pId);
			break;
	}
}


void RemoteReaderManagerPlugIn::onDispatcherClosed(GlobalStatus::Code pCloseCode, const QString& pId)
{
	qCDebug(card_remote) << "RemoteDispatcherClient was closed with:" << pCloseCode;
	removeDispatcher(pId);
}


RemoteReaderManagerPlugIn::RemoteReaderManagerPlugIn()
	: ReaderManagerPlugIn(ReaderManagerPlugInType::REMOTE, true)
	, mScanTimer()
	, mReaderList()
	, mConnectToPairedReaders(true)
	, mConnectionCheckInProgress(false)
{
	mScanTimer.setInterval(1000);
	connect(&mScanTimer, &QTimer::timeout, this, &RemoteReaderManagerPlugIn::connectToPairedReaders);
}


RemoteReaderManagerPlugIn::~RemoteReaderManagerPlugIn()
{
	mScanTimer.stop();
	removeAllDispatchers();
}


void RemoteReaderManagerPlugIn::init()
{
	const auto remoteClient = Env::getSingleton<RemoteClient>();
	connect(remoteClient, &RemoteClient::fireNewRemoteDispatcher, this, &RemoteReaderManagerPlugIn::addRemoteDispatcher);
}


QList<Reader*> RemoteReaderManagerPlugIn::getReaders() const
{
	return mReaderList.values();
}


void RemoteReaderManagerPlugIn::addRemoteDispatcher(const QSharedPointer<RemoteDispatcherClient>& pRemoteDispatcher)
{
	Q_ASSERT(pRemoteDispatcher);

	mDispatcherList.insert(pRemoteDispatcher->getId(), pRemoteDispatcher);

	connect(pRemoteDispatcher.data(), &RemoteDispatcherClient::fireContextEstablished, this, &RemoteReaderManagerPlugIn::onContextEstablished);
	connect(pRemoteDispatcher.data(), &RemoteDispatcherClient::fireReceived, this, &RemoteReaderManagerPlugIn::onRemoteMessage);
	connect(pRemoteDispatcher.data(), &RemoteDispatcherClient::fireClosed, this, &RemoteReaderManagerPlugIn::onDispatcherClosed);

	QMetaObject::invokeMethod(pRemoteDispatcher.data(), [pRemoteDispatcher] {
				const QSharedPointer<const IfdEstablishContext>& establishContext = QSharedPointer<IfdEstablishContext>::create(QStringLiteral("IFDInterface_WebSocket_v0"), DeviceInfo::getName());
				pRemoteDispatcher->send(establishContext);
			}, Qt::QueuedConnection);
}


void RemoteReaderManagerPlugIn::startScan(bool pAutoConnect)
{
	mConnectToPairedReaders = pAutoConnect;
	if (!mConnectToPairedReaders)
	{
		removeAllDispatchers();
	}

	const auto remoteClient = Env::getSingleton<RemoteClient>();
	connect(remoteClient, &RemoteClient::fireDeviceAppeared, this, &RemoteReaderManagerPlugIn::connectToPairedReaders, Qt::UniqueConnection);

	mScanTimer.start();
	QMetaObject::invokeMethod(remoteClient, &RemoteClient::startDetection, Qt::QueuedConnection);
	connectToPairedReaders();
	ReaderManagerPlugIn::startScan(pAutoConnect);
}


void RemoteReaderManagerPlugIn::stopScan(const QString& pError)
{
	const auto remoteClient = Env::getSingleton<RemoteClient>();
	disconnect(remoteClient, &RemoteClient::fireDeviceAppeared, this, &RemoteReaderManagerPlugIn::connectToPairedReaders);
	mScanTimer.stop();
	QMetaObject::invokeMethod(remoteClient, &RemoteClient::stopDetection, Qt::QueuedConnection);
	removeAllDispatchers();
	ReaderManagerPlugIn::stopScan(pError);
}

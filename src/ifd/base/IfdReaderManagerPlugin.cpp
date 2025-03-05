/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "IfdReaderManagerPlugin.h"

#include "AppSettings.h"
#include "IfdReader.h"
#include "messages/IfdError.h"
#include "messages/IfdGetStatus.h"

#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(card_remote)


void IfdReaderManagerPlugin::removeDispatcher(const QByteArray& pId)
{
	const auto& remoteReader = mReadersForDispatcher.values(pId);
	for (const auto& readerName : remoteReader)
	{
		if (readerName.isEmpty())
		{
			continue;
		}

		if (QScopedPointer reader(mReaderList.take(readerName)); reader)
		{
			Q_EMIT fireReaderRemoved(reader->getReaderInfo());
		}
		else
		{
			Q_EMIT fireReaderRemoved(ReaderInfo(readerName));
		}
		mReadersForDispatcher.remove(pId, readerName);
	}

	const auto* dispatcher = mDispatcherList.value(pId).data();
	disconnect(dispatcher, &IfdDispatcherClient::fireContextEstablished, this, &IfdReaderManagerPlugin::onContextEstablished);
	disconnect(dispatcher, &IfdDispatcher::fireReceived, this, &IfdReaderManagerPlugin::onMessage);
	disconnect(dispatcher, &IfdDispatcher::fireClosed, this, &IfdReaderManagerPlugin::onDispatcherClosed);

	mDispatcherList.remove(pId);
}


void IfdReaderManagerPlugin::removeAllDispatchers()
{
	for (const auto& dispatcher : std::as_const(mDispatcherList))
	{
		QMetaObject::invokeMethod(dispatcher.data(), &IfdDispatcher::close, Qt::QueuedConnection);
	}
}


void IfdReaderManagerPlugin::processConnectedReader(const QString& pReaderName, const IfdStatus& pIfdStatus, const QSharedPointer<IfdDispatcherClient>& pDispatcher, const QByteArray& pId)
{
	bool newReader = false;
	if (mReaderList.contains(pReaderName))
	{
		if (auto* reader = mReaderList.value(pReaderName); reader)
		{
			qCDebug(card_remote) << "Update reader" << pReaderName;
			static_cast<IfdReader*>(reader)->updateStatus(pIfdStatus);
			return;
		}
		qCDebug(card_remote) << "Enable reader" << pReaderName;
	}
	else
	{
		qCDebug(card_remote) << "Add reader" << pReaderName;
		newReader = true;
	}

	auto* reader = new IfdReader(getInfo().getPluginType(), pReaderName, pDispatcher, pIfdStatus);
	connect(reader, &IfdReader::fireCardInserted, this, &IfdReaderManagerPlugin::fireCardInserted);
	connect(reader, &IfdReader::fireCardRemoved, this, &IfdReaderManagerPlugin::fireCardRemoved);
	connect(reader, &IfdReader::fireCardInfoChanged, this, &IfdReaderManagerPlugin::fireCardInfoChanged);
	connect(reader, &IfdReader::fireReaderPropertiesUpdated, this, &IfdReaderManagerPlugin::fireReaderPropertiesUpdated);

	mReaderList.insert(pReaderName, reader);
	if (newReader)
	{
		mReadersForDispatcher.insert(pId, pReaderName);
		Q_EMIT fireReaderAdded(reader->getReaderInfo());
	}
	else
	{
		Q_EMIT fireReaderPropertiesUpdated(reader->getReaderInfo());
	}

	// Also update card
	reader->updateStatus(pIfdStatus);
}


void IfdReaderManagerPlugin::handleIFDStatus(const QJsonObject& pJsonObject, const QByteArray& pId)
{
	const auto it = mDispatcherList.constFind(pId);
	if (it == mDispatcherList.constEnd())
	{
		return;
	}
	const auto& dispatcher = *it;
	IfdStatus ifdStatus(pJsonObject);

	const QString& contextHandle = dispatcher->getContextHandle();
	const QString& readerName = ifdStatus.getSlotName() + contextHandle;

	if (ifdStatus.getConnectedReader())
	{
		processConnectedReader(readerName, ifdStatus, dispatcher, pId);
		return;
	}

	ReaderInfo readerInfo(readerName);
	readerInfo.setBasicReader(!ifdStatus.hasPinPad());
	readerInfo.setMaxApduLength(ifdStatus.getMaxApduLength());
	if (mReaderList.contains(readerName))
	{
		qCDebug(card_remote) << "Disable reader" << readerName;
		if (QScopedPointer reader(mReaderList.take(readerName)); reader)
		{
			mReaderList.insert(readerName, nullptr);
		}
		Q_EMIT fireReaderPropertiesUpdated(readerInfo);
		return;
	}

	qCDebug(card_remote) << "Advertise reader" << readerName;
	mReaderList.insert(readerName, nullptr);
	mReadersForDispatcher.insert(pId, readerName);
	Q_EMIT fireReaderAdded(readerInfo);
}


void IfdReaderManagerPlugin::onContextEstablished(const QString& pIfdName, const QByteArray& pId) const
{
	const auto& dispatcher = mDispatcherList.value(pId);

	if (getInfo().getPluginType() == ReaderManagerPluginType::REMOTE_IFD)
	{
		const RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
		QString savedIfdName = settings.getRemoteInfo(pId).getNameEscaped();
		dispatcher->saveRemoteNameInSettings(pIfdName);
		if (savedIfdName != RemoteServiceSettings::escapeDeviceName(pIfdName))
		{
			Q_EMIT getIfdClient()->fireDispatcherChanged(dispatcher);
		}
		if (dispatcher->isPairingConnection())
		{
			QMetaObject::invokeMethod(dispatcher.data(), &IfdDispatcher::close, Qt::QueuedConnection);
			return;
		}
	}

	QMetaObject::invokeMethod(dispatcher.data(), [dispatcher] {
				const QSharedPointer<const IfdGetStatus>& ifdGetStatus = QSharedPointer<IfdGetStatus>::create();
				dispatcher->send(ifdGetStatus);
			}, Qt::QueuedConnection);
}


void IfdReaderManagerPlugin::onMessage(IfdMessageType pMessageType, const QJsonObject& pJsonObject, const QByteArray& pId)
{
	switch (pMessageType)
	{
		case IfdMessageType::IFDError:
		case IfdMessageType::UNDEFINED:
		case IfdMessageType::IFDEstablishContextResponse:
		case IfdMessageType::IFDConnectResponse:
		case IfdMessageType::IFDTransmitResponse:
		case IfdMessageType::IFDDisconnectResponse:
		case IfdMessageType::IFDEstablishPACEChannelResponse:
		case IfdMessageType::IFDDestroyPACEChannelResponse:
		case IfdMessageType::IFDModifyPINResponse:
			break;

		case IfdMessageType::IFDEstablishContext:
		case IfdMessageType::IFDGetStatus:
		case IfdMessageType::IFDConnect:
		case IfdMessageType::IFDDisconnect:
		case IfdMessageType::IFDTransmit:
		case IfdMessageType::IFDEstablishPACEChannel:
		case IfdMessageType::IFDDestroyPACEChannel:
		case IfdMessageType::IFDModifyPIN:
		{
			qCWarning(card_remote) << "Received an unexpected message of type:" << pMessageType;
			const auto& dispatcher = mDispatcherList.value(pId);
			QMetaObject::invokeMethod(dispatcher.data(), [dispatcher] {
						const QSharedPointer<const IfdError>& errorMessage = QSharedPointer<IfdError>::create(QString(), ECardApiResult::Minor::AL_Unknown_API_Function);
						dispatcher->send(errorMessage);
					}, Qt::QueuedConnection);
			break;
		}

		case IfdMessageType::IFDStatus:
			handleIFDStatus(pJsonObject, pId);
			break;
	}
}


void IfdReaderManagerPlugin::onDispatcherClosed(GlobalStatus::Code pCloseCode, const QByteArray& pId)
{
	qCDebug(card_remote) << "IfdDispatcherClient was closed with:" << pCloseCode;
	removeDispatcher(pId);
}


IfdReaderManagerPlugin::IfdReaderManagerPlugin(ReaderManagerPluginType pPluginType, bool pAvailable, bool pPluginEnabled)
	:   ReaderManagerPlugin(pPluginType, pAvailable, pPluginEnabled)
	, mReadersForDispatcher()
	, mDispatcherList()
	, mReaderList()
{
}


IfdReaderManagerPlugin::~IfdReaderManagerPlugin()
{
	// can't wait for removeAllDispatchers answer because were are in dtor
	// and must delete Reader* of mReaderList.
	const auto list = mDispatcherList.keys();
	for (const auto& id : list)
	{
		removeDispatcher(id);
	}
}


void IfdReaderManagerPlugin::init()
{
	ReaderManagerPlugin::init();

	const auto ifdClient = getIfdClient();
	connect(ifdClient, &IfdClient::fireNewDispatcher, this, &IfdReaderManagerPlugin::addDispatcher);
}


QList<Reader*> IfdReaderManagerPlugin::getReaders() const
{
	auto readerList = mReaderList.values();
	readerList.removeAll(nullptr);
	return readerList;
}


void IfdReaderManagerPlugin::addDispatcher(const QSharedPointer<IfdDispatcherClient>& pDispatcher)
{
	Q_ASSERT(pDispatcher);

	mDispatcherList.insert(pDispatcher->getId(), pDispatcher);

	connect(pDispatcher.data(), &IfdDispatcherClient::fireContextEstablished, this, &IfdReaderManagerPlugin::onContextEstablished);
	connect(pDispatcher.data(), &IfdDispatcherClient::fireReceived, this, &IfdReaderManagerPlugin::onMessage);
	connect(pDispatcher.data(), &IfdDispatcherClient::fireClosed, this, &IfdReaderManagerPlugin::onDispatcherClosed);

	QMetaObject::invokeMethod(pDispatcher.data(), &IfdDispatcherClient::sendEstablishContext, Qt::QueuedConnection);
}


void IfdReaderManagerPlugin::insert(const QString& pReaderName, const QVariant& pData)
{
	Reader* const reader = mReaderList.value(pReaderName);
	if (!reader || !reader->getReaderInfo().isInsertable())
	{
		qCDebug(card_remote) << "Skipping insert because reader is not connected or there is no card available";
		return;
	}

	reader->insertCard(pData);
}


void IfdReaderManagerPlugin::startScan(bool pAutoConnect)
{
	const auto ifdClient = getIfdClient();
	QMetaObject::invokeMethod(ifdClient, &IfdClient::startDetection, Qt::QueuedConnection);
	ReaderManagerPlugin::startScan(pAutoConnect);
}


void IfdReaderManagerPlugin::stopScan(const QString& pError)
{
	const auto ifdClient = getIfdClient();
	QMetaObject::invokeMethod(ifdClient, &IfdClient::stopDetection, Qt::QueuedConnection);
	removeAllDispatchers();
	ReaderManagerPlugin::stopScan(pError);
}


const QMap<QByteArray, QSharedPointer<IfdDispatcherClient>>& IfdReaderManagerPlugin::getDispatchers() const
{
	return mDispatcherList;
}

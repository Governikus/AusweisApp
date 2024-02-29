/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

#include "IfdReaderManagerPlugIn.h"

#include "AppSettings.h"
#include "IfdReader.h"
#include "messages/IfdError.h"
#include "messages/IfdGetStatus.h"
#include "messages/IfdStatus.h"

#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(card_remote)


void IfdReaderManagerPlugIn::removeDispatcher(const QString& pId)
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
	disconnect(dispatcher, &IfdDispatcherClient::fireContextEstablished, this, &IfdReaderManagerPlugIn::onContextEstablished);
	disconnect(dispatcher, &IfdDispatcher::fireReceived, this, &IfdReaderManagerPlugIn::onMessage);
	disconnect(dispatcher, &IfdDispatcher::fireClosed, this, &IfdReaderManagerPlugIn::onDispatcherClosed);

	mDispatcherList.remove(pId);
}


void IfdReaderManagerPlugIn::removeAllDispatchers()
{
	for (const auto& dispatcher : std::as_const(mDispatcherList))
	{
		QMetaObject::invokeMethod(dispatcher.data(), &IfdDispatcher::close, Qt::QueuedConnection);
	}
}


void IfdReaderManagerPlugIn::handleIFDStatus(const QJsonObject& pJsonObject, const QString& pId)
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
		bool newReader = false;
		if (mReaderList.contains(readerName))
		{
			if (auto* reader = mReaderList.value(readerName); reader)
			{
				qCDebug(card_remote) << "Update reader" << readerName;
				static_cast<IfdReader*>(reader)->updateStatus(ifdStatus);
				return;
			}
			qCDebug(card_remote) << "Enable reader" << readerName;
		}
		else
		{
			qCDebug(card_remote) << "Add reader" << readerName;
			newReader = true;
		}

		auto reader = new IfdReader(getInfo().getPlugInType(), readerName, dispatcher, ifdStatus);
		connect(reader, &IfdReader::fireCardInserted, this, &IfdReaderManagerPlugIn::fireCardInserted);
		connect(reader, &IfdReader::fireCardRemoved, this, &IfdReaderManagerPlugIn::fireCardRemoved);
		connect(reader, &IfdReader::fireCardInfoChanged, this, &IfdReaderManagerPlugIn::fireCardInfoChanged);
		connect(reader, &IfdReader::fireReaderPropertiesUpdated, this, &IfdReaderManagerPlugIn::fireReaderPropertiesUpdated);

		mReaderList.insert(readerName, reader);
		if (newReader)
		{
			mReadersForDispatcher.insert(pId, readerName);
			Q_EMIT fireReaderAdded(reader->getReaderInfo());
		}
		else
		{
			Q_EMIT fireReaderPropertiesUpdated(reader->getReaderInfo());
		}

		// Also update card
		reader->updateStatus(ifdStatus);

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


void IfdReaderManagerPlugIn::onContextEstablished(const QString& pIfdName, const QString& pId) const
{
	const auto& dispatcher = mDispatcherList.value(pId);

	if (getInfo().getPlugInType() == ReaderManagerPlugInType::REMOTE_IFD)
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


void IfdReaderManagerPlugIn::onMessage(IfdMessageType pMessageType, const QJsonObject& pJsonObject, const QString& pId)
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
		case IfdMessageType::IFDModifyPINResponse:
			break;

		case IfdMessageType::IFDEstablishContext:
		case IfdMessageType::IFDGetStatus:
		case IfdMessageType::IFDConnect:
		case IfdMessageType::IFDDisconnect:
		case IfdMessageType::IFDTransmit:
		case IfdMessageType::IFDEstablishPACEChannel:
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


void IfdReaderManagerPlugIn::onDispatcherClosed(GlobalStatus::Code pCloseCode, const QString& pId)
{
	qCDebug(card_remote) << "IfdDispatcherClient was closed with:" << pCloseCode;
	removeDispatcher(pId);
}


IfdReaderManagerPlugIn::IfdReaderManagerPlugIn(ReaderManagerPlugInType pPlugInType, bool pAvailable, bool pPlugInEnabled)
	:   ReaderManagerPlugIn(pPlugInType, pAvailable, pPlugInEnabled)
	, mReadersForDispatcher()
	, mDispatcherList()
	, mReaderList()
{
}


IfdReaderManagerPlugIn::~IfdReaderManagerPlugIn()
{
	// can't wait for removeAllDispatchers answer because were are in dtor
	// and must delete Reader* of mReaderList.
	const auto list = mDispatcherList.keys();
	for (const auto& id : list)
	{
		removeDispatcher(id);
	}
}


void IfdReaderManagerPlugIn::init()
{
	ReaderManagerPlugIn::init();

	const auto ifdClient = getIfdClient();
	connect(ifdClient, &IfdClient::fireNewDispatcher, this, &IfdReaderManagerPlugIn::addDispatcher);
}


QList<Reader*> IfdReaderManagerPlugIn::getReaders() const
{
	auto readerList = mReaderList.values();
	readerList.removeAll(nullptr);
	return readerList;
}


void IfdReaderManagerPlugIn::addDispatcher(const QSharedPointer<IfdDispatcherClient>& pDispatcher)
{
	Q_ASSERT(pDispatcher);

	mDispatcherList.insert(pDispatcher->getId(), pDispatcher);

	connect(pDispatcher.data(), &IfdDispatcherClient::fireContextEstablished, this, &IfdReaderManagerPlugIn::onContextEstablished);
	connect(pDispatcher.data(), &IfdDispatcherClient::fireReceived, this, &IfdReaderManagerPlugIn::onMessage);
	connect(pDispatcher.data(), &IfdDispatcherClient::fireClosed, this, &IfdReaderManagerPlugIn::onDispatcherClosed);

	QMetaObject::invokeMethod(pDispatcher.data(), &IfdDispatcherClient::sendEstablishContext, Qt::QueuedConnection);
}


void IfdReaderManagerPlugIn::insert(const QString& pReaderName, const QVariant& pData)
{
	Reader* const reader = mReaderList.value(pReaderName);
	if (!reader || !reader->getReaderInfo().isInsertable())
	{
		qCDebug(card_remote) << "Skipping insert because reader is not connected or there is no card available";
		return;
	}

	reader->insertCard(pData);
}


void IfdReaderManagerPlugIn::startScan(bool pAutoConnect)
{
	const auto ifdClient = getIfdClient();
	QMetaObject::invokeMethod(ifdClient, &IfdClient::startDetection, Qt::QueuedConnection);
	ReaderManagerPlugIn::startScan(pAutoConnect);
}


void IfdReaderManagerPlugIn::stopScan(const QString& pError)
{
	const auto ifdClient = getIfdClient();
	QMetaObject::invokeMethod(ifdClient, &IfdClient::stopDetection, Qt::QueuedConnection);
	removeAllDispatchers();
	ReaderManagerPlugIn::stopScan(pError);
}


const QMap<QString, QSharedPointer<IfdDispatcherClient>>& IfdReaderManagerPlugIn::getDispatchers() const
{
	return mDispatcherList;
}

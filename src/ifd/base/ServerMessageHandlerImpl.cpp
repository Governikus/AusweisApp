/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

#include "ServerMessageHandlerImpl.h"

#include "AppSettings.h"
#include "FuncUtils.h"
#include "Randomizer.h"
#include "ReaderManager.h"
#include "messages/IfdConnect.h"
#include "messages/IfdConnectResponse.h"
#include "messages/IfdDisconnect.h"
#include "messages/IfdDisconnectResponse.h"
#include "messages/IfdError.h"
#include "messages/IfdEstablishPaceChannel.h"
#include "messages/IfdEstablishPaceChannelResponse.h"
#include "messages/IfdGetStatus.h"
#include "messages/IfdModifyPin.h"
#include "messages/IfdModifyPinResponse.h"
#include "messages/IfdStatus.h"
#include "messages/IfdTransmit.h"
#include "messages/IfdTransmitResponse.h"
#include "pinpad/PinModifyOutput.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


namespace governikus
{

template<> ServerMessageHandler* createNewObject<ServerMessageHandler*, QSharedPointer<governikus::DataChannel>&, QList<governikus::ReaderManagerPlugInType>&>
	(QSharedPointer<DataChannel>& pChannel, QList<ReaderManagerPlugInType>& pAllowedPlugInTypes)
{
	return new ServerMessageHandlerImpl(pChannel, pAllowedPlugInTypes);
}


ServerMessageHandlerImpl::ServerMessageHandlerImpl(const QSharedPointer<DataChannel>& pDataChannel,
		const QList<ReaderManagerPlugInType>& pAllowedTypes)
	: ServerMessageHandler()
	, mDispatcher(Env::create<IfdDispatcherServer*>(pDataChannel), &QObject::deleteLater)
	, mAllowedPlugInTypes(pAllowedTypes)
	, mAllowedCardTypes(pAllowedTypes)
	, mCardConnections()
{
	connect(mDispatcher.data(), &IfdDispatcherServer::fireReceived, this, &ServerMessageHandlerImpl::onMessage);
	connect(mDispatcher.data(), &IfdDispatcherServer::fireClosed, this, &ServerMessageHandlerImpl::onClosed);
	connect(mDispatcher.data(), &IfdDispatcherServer::fireNameChanged, this, &ServerMessageHandler::fireNameChanged);

	connect(mDispatcher.data(), &IfdDispatcherServer::fireContextEstablished, this, [this] {
			const auto readerManager = Env::getSingleton<ReaderManager>();
			connect(readerManager, &ReaderManager::fireReaderAdded, this, &ServerMessageHandlerImpl::onReaderChanged, Qt::UniqueConnection);
			connect(readerManager, &ReaderManager::fireReaderRemoved, this, &ServerMessageHandlerImpl::onReaderRemoved, Qt::UniqueConnection);
			connect(readerManager, &ReaderManager::fireReaderPropertiesUpdated, this, &ServerMessageHandlerImpl::onReaderChanged, Qt::UniqueConnection);
			connect(readerManager, &ReaderManager::fireCardInserted, this, &ServerMessageHandlerImpl::onReaderChanged, Qt::UniqueConnection);
			connect(readerManager, &ReaderManager::fireCardRemoved, this, &ServerMessageHandlerImpl::onReaderChanged, Qt::UniqueConnection);
		});
}


void ServerMessageHandlerImpl::handleIfdGetStatus(const QJsonObject& pJsonObject)
{
	const IfdGetStatus ifdGetStatus(pJsonObject);
	const auto readerManager = Env::getSingleton<ReaderManager>();

	if (!ifdGetStatus.getSlotName().isEmpty())
	{
		const auto& readerInfo = readerManager->getReaderInfo(ifdGetStatus.getSlotName());
		if (!readerInfo.isValid())
		{
			qCWarning(ifd) << "Unknown reader" << ifdGetStatus.getSlotName();
			const auto& response = QSharedPointer<IfdConnectResponse>::create(ifdGetStatus.getSlotName(), ECardApiResult::Minor::IFDL_UnknownSlot);
			mDispatcher->send(response);
			return;
		}

		sendIfdStatus(readerInfo);
		return;
	}

	const auto& readerInfos = readerManager->getReaderInfos();
	for (const auto& readerInfo : readerInfos)
	{
		if (!readerInfo.isBasicReader())
		{
			continue;
		}

		if (!mAllowedPlugInTypes.contains(readerInfo.getPlugInType()))
		{
			continue;
		}

		sendIfdStatus(readerInfo);
	}
}


void ServerMessageHandlerImpl::handleIfdConnect(const QJsonObject& pJsonObject)
{
	const IfdConnect ifdConnect(pJsonObject);
	const auto readerManager = Env::getSingleton<ReaderManager>();

	const auto& info = readerManager->getReaderInfo(ifdConnect.getSlotName());
	if (!info.isValid())
	{
		qCWarning(ifd) << "Unknown reader" << ifdConnect.getSlotName();
		const auto& response = QSharedPointer<IfdConnectResponse>::create(ifdConnect.getSlotName(), ECardApiResult::Minor::IFDL_UnknownSlot);
		mDispatcher->send(response);
		return;
	}

	if (!info.hasEid())
	{
		qCWarning(ifd) << "Cannot determine eID card for reader" << ifdConnect.getSlotName();
		const auto& response = QSharedPointer<IfdConnectResponse>::create(ifdConnect.getSlotName(), ECardApiResult::Minor::AL_Unknown_Error);
		mDispatcher->send(response);
		return;
	}

	const auto& connections = mCardConnections.values();
	const auto& slotNames = map<QSharedPointer<CardConnection>, QString>([](const QSharedPointer<CardConnection>& c){
			return c ? c->getReaderInfo().getName() : QString();
		}, connections);
	if (slotNames.contains(ifdConnect.getSlotName()))
	{
		qCWarning(ifd) << "Card is already connected" << ifdConnect.getSlotName();
		const auto& response = QSharedPointer<IfdConnectResponse>::create(ifdConnect.getSlotName(), ECardApiResult::Minor::IFDL_IFD_SharingViolation);
		mDispatcher->send(response);
		return;
	}

	qCDebug(ifd) << "Connect card" << ifdConnect.getSlotName();
	readerManager->callCreateCardConnectionCommand(ifdConnect.getSlotName(), this, &ServerMessageHandlerImpl::onCreateCardConnectionCommandDone);
}


void ServerMessageHandlerImpl::onCreateCardConnectionCommandDone(QSharedPointer<CreateCardConnectionCommand> pCommand)
{
	qCDebug(ifd) << "Card connection command completed";
	if (pCommand->getCardConnection() == nullptr)
	{
		qCWarning(ifd) << "Cannot connect card" << pCommand->getReaderName();
		const auto& response = QSharedPointer<IfdConnectResponse>::create(pCommand->getReaderName(), ECardApiResult::Minor::AL_Unknown_Error);
		mDispatcher->send(response);
		return;
	}

	QString slotHandle = Randomizer::getInstance().createUuid().toString();
	qCInfo(ifd) << "Card successfully connected" << pCommand->getReaderName() << ", using handle " << slotHandle;
	mCardConnections.insert(slotHandle, pCommand->getCardConnection());

	const auto& response = QSharedPointer<IfdConnectResponse>::create(slotHandle);
	mDispatcher->send(response);

	Q_EMIT fireCardConnected(pCommand->getCardConnection());
}


QString ServerMessageHandlerImpl::slotHandleForReaderName(const QString& pReaderName) const
{
	const auto& slotHandles = mCardConnections.keys();
	for (const auto& slotHandle : slotHandles)
	{
		if (mCardConnections[slotHandle]->getReaderInfo().getName() == pReaderName)
		{
			return slotHandle;
		}
	}
	return QString();
}


void ServerMessageHandlerImpl::handleIfdDisconnect(const QJsonObject& pJsonObject)
{
	const IfdDisconnect ifdDisconnect(pJsonObject);
	const QString& slotHandle = ifdDisconnect.getSlotHandle();

	if (!mCardConnections.contains(slotHandle))
	{
		qCWarning(ifd) << "Card is not connected" << slotHandle;
		const auto& response = QSharedPointer<IfdDisconnectResponse>::create(slotHandle, ECardApiResult::Minor::IFDL_InvalidSlotHandle);
		mDispatcher->send(response);
		return;
	}

	const auto cardConnection = mCardConnections.take(slotHandle);
	qCInfo(ifd) << "Card successfully disconnected" << slotHandle;
	const auto& response = QSharedPointer<IfdDisconnectResponse>::create(slotHandle);
	mDispatcher->send(response);

	Q_EMIT fireCardDisconnected(cardConnection);
}


void ServerMessageHandlerImpl::handleIfdTransmit(const QJsonObject& pJsonObject)
{
	const IfdTransmit ifdTransmit(pJsonObject);
	const QString& slotHandle = ifdTransmit.getSlotHandle();

	if (!mCardConnections.contains(slotHandle))
	{
		qCWarning(ifd) << "Card is not connected" << slotHandle;
		const auto& response = QSharedPointer<IfdTransmitResponse>::create(slotHandle, QByteArray(), ECardApiResult::Minor::IFDL_InvalidSlotHandle);
		mDispatcher->send(response);
		return;
	}

	const QSharedPointer<CardConnection>& cardConnection = mCardConnections.value(slotHandle);

	const QString& progressMessage = ifdTransmit.getDisplayText();
	if (!progressMessage.isNull())
	{
		cardConnection->setProgressMessage(progressMessage);
		Q_EMIT fireDisplayTextChanged(progressMessage);
	}

	qCDebug(ifd) << "Transmit card APDU for" << slotHandle;
	InputAPDUInfo inputApduInfo(ifdTransmit.getInputApdu());
	cardConnection->callTransmitCommand(this, &ServerMessageHandlerImpl::onTransmitCardCommandDone, {inputApduInfo}, slotHandle);
}


void ServerMessageHandlerImpl::handleIfdEstablishPaceChannel(const QJsonObject& pJsonObject)
{
	const auto& ifdEstablishPaceChannel = QSharedPointer<IfdEstablishPaceChannel>::create(pJsonObject);
	const QString& slotHandle = ifdEstablishPaceChannel->getSlotHandle();

	if (!mCardConnections.contains(slotHandle))
	{
		qCWarning(ifd) << "Card is not connected" << slotHandle;
		const auto& response = QSharedPointer<IfdEstablishPaceChannelResponse>::create(slotHandle, EstablishPaceChannelOutput(), ECardApiResult::Minor::IFDL_InvalidSlotHandle);
		mDispatcher->send(response);
		return;
	}

	QSharedPointer<CardConnection> cardConnection = mCardConnections.value(slotHandle);

	const bool isBasicReader = cardConnection->getReaderInfo().isBasicReader();
	const bool pinPadMode = Env::getSingleton<AppSettings>()->getRemoteServiceSettings().getPinPadMode();
	if (isBasicReader && !pinPadMode)
	{
		qCWarning(ifd) << "EstablishPaceChannel is only available in pin pad mode.";
		const auto& response = QSharedPointer<IfdEstablishPaceChannelResponse>::create(slotHandle, EstablishPaceChannelOutput(), ECardApiResult::Minor::AL_Unknown_Error);
		mDispatcher->send(response);
		return;
	}

	Q_EMIT fireEstablishPaceChannel(ifdEstablishPaceChannel, cardConnection);
}


void ServerMessageHandlerImpl::sendEstablishPaceChannelResponse(const QString& pSlotHandle, const EstablishPaceChannelOutput& pChannelOutput)
{
	if (pChannelOutput.getPaceReturnCode() == CardReturnCode::UNKNOWN)
	{
		const auto& response = QSharedPointer<IfdEstablishPaceChannelResponse>::create(pSlotHandle, pChannelOutput, ECardApiResult::Minor::AL_Unknown_Error);
		mDispatcher->send(response);
		return;
	}

	if (pChannelOutput.getPaceReturnCode() == CardReturnCode::CARD_NOT_FOUND)
	{
		const auto& response = QSharedPointer<IfdEstablishPaceChannelResponse>::create(pSlotHandle, pChannelOutput, ECardApiResult::Minor::IFDL_Terminal_NoCard);
		mDispatcher->send(response);
		return;
	}

	const auto& response = QSharedPointer<IfdEstablishPaceChannelResponse>::create(pSlotHandle, pChannelOutput);
	mDispatcher->send(response);
}


void ServerMessageHandlerImpl::handleIfdModifyPIN(const QJsonObject& pJsonObject)
{
	const auto& ifdModifyPin = QSharedPointer<IfdModifyPin>::create(pJsonObject);
	const QString slotHandle = ifdModifyPin->getSlotHandle();
	const bool pinPadMode = Env::getSingleton<AppSettings>()->getRemoteServiceSettings().getPinPadMode();
	if (!pinPadMode)
	{
		qCWarning(ifd) << "ModifyPin is only available in pin pad mode.";
		const auto& response = QSharedPointer<IfdModifyPinResponse>::create(slotHandle, QByteArray(), ECardApiResult::Minor::AL_Unknown_Error);
		mDispatcher->send(response);
		return;
	}

	if (!mCardConnections.contains(slotHandle))
	{
		qCWarning(ifd) << "Card is not connected" << slotHandle;
		const auto& response = QSharedPointer<IfdModifyPinResponse>::create(slotHandle, QByteArray(), ECardApiResult::Minor::IFDL_InvalidSlotHandle);
		mDispatcher->send(response);
		return;
	}

	Q_EMIT fireModifyPin(ifdModifyPin, mCardConnections[slotHandle]);
}


void ServerMessageHandlerImpl::sendIfdStatus(const ReaderInfo& pReaderInfo)
{
	if (!mDispatcher->getContextHandle().isEmpty())
	{
		const bool isCardAllowed = mAllowedCardTypes.contains(pReaderInfo.getPlugInType());
		mDispatcher->send(QSharedPointer<IfdStatus>::create(pReaderInfo, isCardAllowed));
	}
}


void ServerMessageHandlerImpl::sendModifyPinResponse(const QString& pSlotHandle, const ResponseApdu& pResponseApdu)
{
	PinModifyOutput pinModifyOutput(pResponseApdu);
	const QByteArray& ccid = pinModifyOutput.toCcid();

	ECardApiResult::Minor minor = ECardApiResult::Minor::null;
	switch (pResponseApdu.getStatusCode())
	{
		case StatusCode::SUCCESS:
			break;

		case StatusCode::INPUT_TIMEOUT:
			minor = ECardApiResult::Minor::IFDL_Timeout_Error;
			break;

		case StatusCode::INPUT_CANCELLED:
			minor = ECardApiResult::Minor::IFDL_CancellationByUser;
			break;

		case StatusCode::PASSWORDS_DIFFER:
			minor = ECardApiResult::Minor::IFDL_IO_RepeatedDataMismatch;
			break;

		case StatusCode::PASSWORD_OUTOF_RANGE:
			minor = ECardApiResult::Minor::IFDL_IO_UnknownPINFormat;
			break;

		default:
			minor = pResponseApdu.isEmpty()
					? ECardApiResult::Minor::IFDL_Terminal_NoCard
					: ECardApiResult::Minor::AL_Unknown_Error;
	}

	const auto& response = QSharedPointer<IfdModifyPinResponse>::create(pSlotHandle, ccid, minor);
	mDispatcher->send(response);
}


void ServerMessageHandlerImpl::setAllowedCardTypes(const QList<ReaderManagerPlugInType>& pAllowedCardTypes)
{
	if (mAllowedCardTypes != pAllowedCardTypes)
	{
		mAllowedCardTypes = pAllowedCardTypes;

		const auto& readerInfos = Env::getSingleton<ReaderManager>()->getReaderInfos(ReaderFilter(mAllowedPlugInTypes));
		for (const auto& readerInfo : readerInfos)
		{
			sendIfdStatus(readerInfo);
		}
	}
}


void ServerMessageHandlerImpl::onTransmitCardCommandDone(QSharedPointer<BaseCardCommand> pCommand)
{
	auto transmitCommand = pCommand.staticCast<TransmitCommand>();
	const QString& slotHandle = transmitCommand->getSlotHandle();

	if (transmitCommand->getReturnCode() != CardReturnCode::OK)
	{
		qCWarning(ifd) << "Card transmit for" << slotHandle << "failed" << transmitCommand->getReturnCode();
		const auto& response = QSharedPointer<IfdTransmitResponse>::create(slotHandle, QByteArray(), ECardApiResult::Minor::AL_Unknown_Error);
		mDispatcher->send(response);
		return;
	}

	QByteArray responseApdu;
	Q_ASSERT(transmitCommand->getOutputApduAsHex().size() == 1); // may not happen, see TransmitCommand
	if (transmitCommand->getOutputApduAsHex().size() == 1)
	{
		responseApdu = QByteArray::fromHex(transmitCommand->getOutputApduAsHex().first());
	}
	qCInfo(ifd) << "Card transmit succeeded" << slotHandle;
	const auto& response = QSharedPointer<IfdTransmitResponse>::create(slotHandle, responseApdu);
	mDispatcher->send(response);

	if (transmitCommand->getSecureMessagingStopped())
	{
		Q_EMIT fireSecureMessagingStopped();
	}
}


void ServerMessageHandlerImpl::onClosed()
{
	mCardConnections.clear();

	Q_EMIT fireClosed();
}


void ServerMessageHandlerImpl::onMessage(IfdMessageType pMessageType, const QJsonObject& pJsonObject)
{
	switch (pMessageType)
	{
		case IfdMessageType::IFDError:
		case IfdMessageType::UNDEFINED:
		case IfdMessageType::IFDEstablishContext:
		case IfdMessageType::IFDEstablishContextResponse:
			break;

		case IfdMessageType::IFDStatus:
		case IfdMessageType::IFDConnectResponse:
		case IfdMessageType::IFDDisconnectResponse:
		case IfdMessageType::IFDTransmitResponse:
		case IfdMessageType::IFDEstablishPACEChannelResponse:
		case IfdMessageType::IFDModifyPINResponse:
		{
			qCWarning(ifd) << "Received an unexpected message of type:" << pMessageType;
			const auto& localCopy = mDispatcher;
			QMetaObject::invokeMethod(localCopy.data(), [localCopy] {
					const QSharedPointer<const IfdError>& errorMessage = QSharedPointer<IfdError>::create(QString(), ECardApiResult::Minor::AL_Unknown_API_Function);
					localCopy->send(errorMessage);
				}, Qt::QueuedConnection);
			break;
		}

		case IfdMessageType::IFDGetStatus:
			handleIfdGetStatus(pJsonObject);
			break;

		case IfdMessageType::IFDConnect:
			handleIfdConnect(pJsonObject);
			break;

		case IfdMessageType::IFDTransmit:
			handleIfdTransmit(pJsonObject);
			break;

		case IfdMessageType::IFDDisconnect:
			handleIfdDisconnect(pJsonObject);
			break;

		case IfdMessageType::IFDEstablishPACEChannel:
			handleIfdEstablishPaceChannel(pJsonObject);
			break;

		case IfdMessageType::IFDModifyPIN:
			handleIfdModifyPIN(pJsonObject);
			break;
	}
}


void ServerMessageHandlerImpl::onReaderChanged(const ReaderInfo& pInfo)
{
	if (!mAllowedPlugInTypes.contains(pInfo.getPlugInType()))
	{
		return;
	}

	if (!pInfo.hasEid())
	{
		const QString& slotHandle = slotHandleForReaderName(pInfo.getName());
		if (mCardConnections.remove(slotHandle) > 0)
		{
			qCInfo(ifd) << "Removed CardConnection for" << slotHandle;
		}
	}

	sendIfdStatus(pInfo);
}


void ServerMessageHandlerImpl::onReaderRemoved(const ReaderInfo& pInfo)
{
	if (!mAllowedPlugInTypes.contains(pInfo.getPlugInType()))
	{
		return;
	}

	sendIfdStatus(pInfo);
}


} // namespace governikus

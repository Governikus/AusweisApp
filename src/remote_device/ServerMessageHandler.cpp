/*!
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#include "ServerMessageHandler.h"

#include "AppSettings.h"
#include "Env.h"
#include "FuncUtils.h"
#include "messages/IfdConnect.h"
#include "messages/IfdConnectResponse.h"
#include "messages/IfdDisconnect.h"
#include "messages/IfdDisconnectResponse.h"
#include "messages/IfdError.h"
#include "messages/IfdEstablishContext.h"
#include "messages/IfdEstablishPaceChannel.h"
#include "messages/IfdEstablishPaceChannelResponse.h"
#include "messages/IfdGetStatus.h"
#include "messages/IfdModifyPin.h"
#include "messages/IfdModifyPinResponse.h"
#include "messages/IfdStatus.h"
#include "messages/IfdTransmit.h"
#include "messages/IfdTransmitResponse.h"
#include "PinModifyOutput.h"
#include "ReaderManager.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


namespace governikus
{

template<> ServerMessageHandler* createNewObject<ServerMessageHandler*, QSharedPointer<governikus::DataChannel>&>(QSharedPointer<DataChannel>& pChannel)
{
	return new ServerMessageHandlerImpl(pChannel);
}


ServerMessageHandler::~ServerMessageHandler()
{
}


ServerMessageHandlerImpl::ServerMessageHandlerImpl(const QSharedPointer<DataChannel>& pDataChannel)
	: ServerMessageHandler()
	, mReaderManager(Env::getSingleton<ReaderManager>())
	, mRemoteDispatcher(Env::create<RemoteDispatcherServer*>(pDataChannel), &QObject::deleteLater)
	, mCardConnections()
{
	connect(mRemoteDispatcher.data(), &RemoteDispatcherServer::fireReceived, this, &ServerMessageHandlerImpl::onRemoteMessage);
	connect(mRemoteDispatcher.data(), &RemoteDispatcherServer::fireClosed, this, &ServerMessageHandlerImpl::onClosed);

	connect(mRemoteDispatcher.data(), &RemoteDispatcherServer::fireContextEstablished, this, [this] {
				const auto readerManager = Env::getSingleton<ReaderManager>();
				connect(readerManager, &ReaderManager::fireReaderAdded, this, &ServerMessageHandlerImpl::onReaderChanged, Qt::UniqueConnection);
				connect(readerManager, &ReaderManager::fireReaderRemoved, this, &ServerMessageHandlerImpl::onReaderRemoved, Qt::UniqueConnection);
				connect(readerManager, &ReaderManager::fireReaderPropertiesUpdated, this, &ServerMessageHandlerImpl::onReaderChanged, Qt::UniqueConnection);
				connect(readerManager, &ReaderManager::fireCardInserted, this, &ServerMessageHandlerImpl::onReaderChanged, Qt::UniqueConnection);
				connect(readerManager, &ReaderManager::fireCardRemoved, this, &ServerMessageHandlerImpl::onReaderChanged, Qt::UniqueConnection);
				connect(readerManager, &ReaderManager::fireCardRetryCounterChanged, this, &ServerMessageHandlerImpl::onReaderChanged, Qt::UniqueConnection);
			});
}


void ServerMessageHandlerImpl::handleIfdGetStatus(const QJsonObject& pJsonObject)
{
	const IfdGetStatus ifdGetStatus(pJsonObject);

	if (!ifdGetStatus.getSlotName().isEmpty())
	{
		const auto& readerInfo = mReaderManager->getReaderInfo(ifdGetStatus.getSlotName());
		const auto& ifdStatusMsg = QSharedPointer<IfdStatus>::create(readerInfo);
		mRemoteDispatcher->send(ifdStatusMsg);

		return;
	}

	const auto& readerInfos = mReaderManager->getReaderInfos();
	for (const auto& readerInfo : readerInfos)
	{
		if (!readerInfo.isBasicReader())
		{
			continue;
		}

		const auto& ifdStatusMsg = QSharedPointer<IfdStatus>::create(readerInfo);
		mRemoteDispatcher->send(ifdStatusMsg);
	}
}


void ServerMessageHandlerImpl::handleIfdConnect(const QJsonObject& pJsonObject)
{
	const IfdConnect ifdConnect(pJsonObject);

	const auto& info = mReaderManager->getReaderInfo(ifdConnect.getSlotName());
	if (!info.isConnected())
	{
		qCWarning(remote_device) << "Unknown reader" << ifdConnect.getSlotName();
		const auto& response = QSharedPointer<IfdConnectResponse>::create(ifdConnect.getSlotName(), ECardApiResult::Minor::IFDL_UnknownSlot);
		mRemoteDispatcher->send(response);
		return;
	}

	if (!info.hasEidCard())
	{
		qCWarning(remote_device) << "Cannot determine eID card for reader" << ifdConnect.getSlotName();
		const auto& response = QSharedPointer<IfdConnectResponse>::create(ifdConnect.getSlotName(), ECardApiResult::Minor::AL_Unknown_Error);
		mRemoteDispatcher->send(response);
		return;
	}

	const auto& connections = mCardConnections.values();
	const auto& slotNames = map<QSharedPointer<CardConnection>, QString>([](const QSharedPointer<CardConnection>& c){
				return c ? c->getReaderInfo().getName() : QString();
			}, connections);
	if (slotNames.contains(ifdConnect.getSlotName()))
	{
		qCWarning(remote_device) << "Card is already connected" << ifdConnect.getSlotName();
		const auto& response = QSharedPointer<IfdConnectResponse>::create(ifdConnect.getSlotName(), ECardApiResult::Minor::IFDL_IFD_SharingViolation);
		mRemoteDispatcher->send(response);
		return;
	}

	qCDebug(remote_device) << "Connect card" << ifdConnect.getSlotName();
	mReaderManager->callCreateCardConnectionCommand(ifdConnect.getSlotName(), this, &ServerMessageHandlerImpl::onCreateCardConnectionCommandDone);
}


void ServerMessageHandlerImpl::onCreateCardConnectionCommandDone(QSharedPointer<CreateCardConnectionCommand> pCommand)
{
	qCDebug(remote_device) << "Card connection command completed";
	if (pCommand->getCardConnection() == nullptr)
	{
		qCWarning(remote_device) << "Cannot connect card" << pCommand->getReaderName();
		const auto& response = QSharedPointer<IfdConnectResponse>::create(pCommand->getReaderName(), ECardApiResult::Minor::AL_Unknown_Error);
		mRemoteDispatcher->send(response);
		return;
	}

	QString slotHandle = QUuid::createUuid().toString();
	qCInfo(remote_device) << "Card successfully connected" << pCommand->getReaderName() << ", using handle " << slotHandle;
	mCardConnections.insert(slotHandle, pCommand->getCardConnection());

	const auto& response = QSharedPointer<IfdConnectResponse>::create(slotHandle);
	mRemoteDispatcher->send(response);

	Q_EMIT fireCardConnectionEstablished(pCommand->getCardConnection());
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


QString ServerMessageHandlerImpl::convertSlotHandleBackwardsCompatibility(const QString& pReaderName) const
{
	if (!mCardConnections.contains(pReaderName))
	{
		const QString& slotHandle = slotHandleForReaderName(pReaderName);
		if (!slotHandle.isEmpty())
		{
			return slotHandle;
		}
	}
	return pReaderName;
}


void ServerMessageHandlerImpl::handleIfdDisconnect(const QJsonObject& pJsonObject)
{
	const IfdDisconnect ifdDisconnect(pJsonObject);
	QString slotHandle = ifdDisconnect.getSlotHandle();
	slotHandle = convertSlotHandleBackwardsCompatibility(slotHandle);

	if (!mCardConnections.contains(slotHandle))
	{
		qCWarning(remote_device) << "Card is not connected" << slotHandle;
		const auto& response = QSharedPointer<IfdDisconnectResponse>::create(slotHandle, ECardApiResult::Minor::IFDL_InvalidSlotHandle);
		mRemoteDispatcher->send(response);
		return;
	}

	mCardConnections.remove(slotHandle);
	qCInfo(remote_device) << "Card successfully disconnected" << slotHandle;
	const auto& response = QSharedPointer<IfdDisconnectResponse>::create(slotHandle);
	mRemoteDispatcher->send(response);
}


void ServerMessageHandlerImpl::handleIfdTransmit(const QJsonObject& pJsonObject)
{
	const IfdTransmit ifdTransmit(pJsonObject);
	QString slotHandle = ifdTransmit.getSlotHandle();
	slotHandle = convertSlotHandleBackwardsCompatibility(slotHandle);

	if (!mCardConnections.contains(slotHandle))
	{
		qCWarning(remote_device) << "Card is not connected" << slotHandle;
		const auto& response = QSharedPointer<IfdTransmitResponse>::create(slotHandle, QByteArray(), ECardApiResult::Minor::IFDL_InvalidSlotHandle);
		mRemoteDispatcher->send(response);
		return;
	}

	const QSharedPointer<CardConnection>& cardConnection = mCardConnections.value(slotHandle);
	const auto& commandApdu = ifdTransmit.getInputApdu();

	const bool pinPadMode = Env::getSingleton<AppSettings>()->getRemoteServiceSettings().getPinPadMode();
	if (pinPadMode && CommandApdu::isSecureMessaging(commandApdu))
	{
		const bool stopped = cardConnection->stopSecureMessaging();
		if (stopped)
		{
			qCDebug(remote_device) << "The eService has established Secure Messaging. Stopping local Secure Messaging.";
			Q_EMIT fireSecureMessagingStopped();
		}
	}

	qCDebug(remote_device) << "Transmit card APDU for" << slotHandle;
	InputAPDUInfo inputApduInfo(commandApdu);
	cardConnection->callTransmitCommand(this, &ServerMessageHandlerImpl::onTransmitCardCommandDone, {inputApduInfo}, slotHandle);
}


void ServerMessageHandlerImpl::handleIfdEstablishPaceChannel(const QJsonObject& pJsonObject)
{
	const auto& ifdEstablishPaceChannel = QSharedPointer<IfdEstablishPaceChannel>::create(pJsonObject);
	QString slotHandle = ifdEstablishPaceChannel->getSlotHandle();
	slotHandle = convertSlotHandleBackwardsCompatibility(slotHandle);

	if (!mCardConnections.contains(slotHandle))
	{
		qCWarning(remote_device) << "Card is not connected" << slotHandle;
		const auto& response = QSharedPointer<IfdEstablishPaceChannelResponse>::create(slotHandle, QByteArray(), ECardApiResult::Minor::IFDL_InvalidSlotHandle);
		mRemoteDispatcher->send(response);
		return;
	}

	QSharedPointer<CardConnection> cardConnection = mCardConnections.value(slotHandle);

	const bool isBasicReader = cardConnection->getReaderInfo().isBasicReader();
	const bool pinPadMode = Env::getSingleton<AppSettings>()->getRemoteServiceSettings().getPinPadMode();
	if (isBasicReader && !pinPadMode)
	{
		qCWarning(remote_device) << "EstablishPaceChannel is only available in pin pad mode.";
		const auto& response = QSharedPointer<IfdEstablishPaceChannelResponse>::create(slotHandle, QByteArray(), ECardApiResult::Minor::AL_Unknown_Error);
		mRemoteDispatcher->send(response);
		return;
	}

	Q_EMIT fireEstablishPaceChannel(ifdEstablishPaceChannel, cardConnection);
}


void ServerMessageHandlerImpl::sendEstablishPaceChannelResponse(const QString& pSlotHandle, const EstablishPaceChannelOutput& pChannelOutput)
{
	const QByteArray& ccid = pChannelOutput.toCcid();
	if (pChannelOutput.getPaceReturnCode() == CardReturnCode::UNKNOWN)
	{
		const auto& response = QSharedPointer<IfdEstablishPaceChannelResponse>::create(pSlotHandle, ccid, ECardApiResult::Minor::AL_Unknown_Error);
		mRemoteDispatcher->send(response);
		return;
	}

	if (pChannelOutput.getPaceReturnCode() == CardReturnCode::CARD_NOT_FOUND)
	{
		const auto& response = QSharedPointer<IfdEstablishPaceChannelResponse>::create(pSlotHandle, ccid, ECardApiResult::Minor::IFDL_Terminal_NoCard);
		mRemoteDispatcher->send(response);
		return;
	}

	const auto& response = QSharedPointer<IfdEstablishPaceChannelResponse>::create(pSlotHandle, ccid);
	mRemoteDispatcher->send(response);
}


void ServerMessageHandlerImpl::handleIfdModifyPIN(const QJsonObject& pJsonObject)
{
	const auto& ifdModifyPin = QSharedPointer<IfdModifyPin>::create(pJsonObject);
	const QString slotHandle = ifdModifyPin->getSlotHandle();
	const bool pinPadMode = Env::getSingleton<AppSettings>()->getRemoteServiceSettings().getPinPadMode();
	if (!pinPadMode)
	{
		qCWarning(remote_device) << "ModifyPin is only available in pin pad mode.";
		const auto& response = QSharedPointer<IfdModifyPinResponse>::create(slotHandle, QByteArray(), ECardApiResult::Minor::AL_Unknown_Error);
		mRemoteDispatcher->send(response);
		return;
	}

	if (!mCardConnections.contains(slotHandle))
	{
		qCWarning(remote_device) << "Card is not connected" << slotHandle;
		const auto& response = QSharedPointer<IfdModifyPinResponse>::create(slotHandle, QByteArray(), ECardApiResult::Minor::IFDL_InvalidSlotHandle);
		mRemoteDispatcher->send(response);
		return;
	}

	Q_EMIT fireModifyPin(ifdModifyPin, mCardConnections[slotHandle]);
}


void ServerMessageHandlerImpl::sendModifyPinResponse(const QString& pSlotHandle, const ResponseApdu& pResponseApdu)
{
	PinModifyOutput pinModifyOutput(pResponseApdu);
	const QByteArray& ccid = pinModifyOutput.toCcid();

	ECardApiResult::Minor minor = ECardApiResult::Minor::null;
	switch (pResponseApdu.getReturnCode())
	{
		case StatusCode::SUCCESS:
			break;

		case StatusCode::EMPTY:
			minor = ECardApiResult::Minor::IFDL_Terminal_NoCard;
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
			minor = ECardApiResult::Minor::AL_Unknown_Error;
	}

	const auto& response = QSharedPointer<IfdModifyPinResponse>::create(pSlotHandle, ccid, minor);
	mRemoteDispatcher->send(response);
}


void ServerMessageHandlerImpl::onTransmitCardCommandDone(QSharedPointer<BaseCardCommand> pCommand)
{
	auto transmitCommand = pCommand.staticCast<TransmitCommand>();
	QString slotHandle = transmitCommand->getSlotHandle();
	slotHandle = convertSlotHandleBackwardsCompatibility(slotHandle);

	if (transmitCommand->getReturnCode() != CardReturnCode::OK)
	{
		qCWarning(remote_device) << "Card transmit for" << slotHandle << "failed" << transmitCommand->getReturnCode();
		const auto& response = QSharedPointer<IfdTransmitResponse>::create(slotHandle, QByteArray(), ECardApiResult::Minor::AL_Unknown_Error);
		mRemoteDispatcher->send(response);
		return;
	}

	QByteArray responseApdu;
	Q_ASSERT(transmitCommand->getOutputApduAsHex().size() == 1); // may not happen, see TransmitCommand
	if (transmitCommand->getOutputApduAsHex().size() == 1)
	{
		responseApdu = QByteArray::fromHex(transmitCommand->getOutputApduAsHex().first());
	}
	qCInfo(remote_device) << "Card transmit succeeded" << slotHandle;
	const auto& response = QSharedPointer<IfdTransmitResponse>::create(slotHandle, responseApdu);
	mRemoteDispatcher->send(response);
}


void ServerMessageHandlerImpl::onClosed()
{
	mCardConnections.clear();

	Q_EMIT fireClosed();
}


void ServerMessageHandlerImpl::onRemoteMessage(RemoteCardMessageType pMessageType, const QJsonObject& pJsonObject)
{
	switch (pMessageType)
	{
		case RemoteCardMessageType::IFDError:
		case RemoteCardMessageType::UNDEFINED:
		case RemoteCardMessageType::IFDEstablishContext:
		case RemoteCardMessageType::IFDEstablishContextResponse:
			break;

		case RemoteCardMessageType::IFDStatus:
		case RemoteCardMessageType::IFDConnectResponse:
		case RemoteCardMessageType::IFDDisconnectResponse:
		case RemoteCardMessageType::IFDTransmitResponse:
		case RemoteCardMessageType::IFDEstablishPACEChannelResponse:
		case RemoteCardMessageType::IFDModifyPINResponse:
		{
			qCWarning(remote_device) << "Received an unexpected message of type:" << pMessageType;
			const auto& localCopy = mRemoteDispatcher;
			QMetaObject::invokeMethod(localCopy.data(), [localCopy] {
						const QSharedPointer<const IfdError>& errorMessage = QSharedPointer<IfdError>::create(QString(), ECardApiResult::Minor::AL_Unkown_API_Function);
						localCopy->send(errorMessage);
					}, Qt::QueuedConnection);
			break;
		}

		case RemoteCardMessageType::IFDGetStatus:
			handleIfdGetStatus(pJsonObject);
			break;

		case RemoteCardMessageType::IFDConnect:
			handleIfdConnect(pJsonObject);
			break;

		case RemoteCardMessageType::IFDTransmit:
			handleIfdTransmit(pJsonObject);
			break;

		case RemoteCardMessageType::IFDDisconnect:
			handleIfdDisconnect(pJsonObject);
			break;

		case RemoteCardMessageType::IFDEstablishPACEChannel:
			handleIfdEstablishPaceChannel(pJsonObject);
			break;

		case RemoteCardMessageType::IFDModifyPIN:
			handleIfdModifyPIN(pJsonObject);
			break;
	}
}


void ServerMessageHandlerImpl::onReaderChanged(const ReaderInfo& pInfo)
{
	if (!pInfo.hasEidCard())
	{
		const QString& slotHandle = slotHandleForReaderName(pInfo.getName());
		if (mCardConnections.remove(slotHandle) > 0)
		{
			qCInfo(remote_device) << "Removed CardConnection for" << slotHandle;
		}
	}

	mRemoteDispatcher->send(QSharedPointer<IfdStatus>::create(pInfo));
}


void ServerMessageHandlerImpl::onReaderRemoved(const ReaderInfo& pInfo)
{
	mRemoteDispatcher->send(QSharedPointer<IfdStatus>::create(pInfo));
}


} // namespace governikus

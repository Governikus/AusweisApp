/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "MockIfdDispatcher.h"

#include "messages/IfdConnect.h"
#include "messages/IfdConnectResponse.h"
#include "messages/IfdDisconnect.h"
#include "messages/IfdDisconnectResponse.h"
#include "messages/IfdEstablishPaceChannel.h"
#include "messages/IfdEstablishPaceChannelResponse.h"
#include "messages/IfdModifyPin.h"
#include "messages/IfdModifyPinResponse.h"
#include "messages/IfdStatus.h"
#include "messages/IfdTransmit.h"
#include "messages/IfdTransmitResponse.h"

#include "MockDataChannel.h"

#include <QTest>
#include <QUuid>


using namespace governikus;


MockIfdDispatcher::MockIfdDispatcher(DispatcherState pState)
	: IfdDispatcherClient(IfdVersion::Version::v2, QSharedPointer<MockDataChannel>::create())
	, mState(pState)
	, mPairing(false)
	, mId(QUuid::createUuid().toByteArray())
	, mContextHandle(QStringLiteral("#TestContext"))
{
}


void MockIfdDispatcher::setPairingConnection(bool pPairing)
{
	mPairing = pPairing;
}


bool MockIfdDispatcher::isPairingConnection() const
{
	return mPairing;
}


QByteArray MockIfdDispatcher::getId() const
{
	return mId;
}


const QString& MockIfdDispatcher::getContextHandle() const
{
	return mContextHandle;
}


void MockIfdDispatcher::send(const QSharedPointer<const IfdMessage>& pMessage)
{
	QVERIFY(pMessage);

	Q_EMIT fireSend(pMessage);

	if (mState == DispatcherState::WithoutReader)
	{
		return;
	}

	if (mState == DispatcherState::ReaderInvalid)
	{
		Q_EMIT fireReceived(IfdMessageType::IFDError,
				QJsonDocument::fromJson("{\n"
										"    \"ContextHandle\": \"TestContext\",\n"
										"    \"OutputData\": \"\",\n"
										"    \"ResponseAPDU\": \"\",\n"
										"    \"ResultCode\": \"\",\n"
										"    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
										"    \"ResultMinor\": null,\n"
										"    \"SlotHandle\": \"SlotHandle\",\n"
										"    \"msg\": \"IFDTest\"\n"
										"}\n").object(), mId);
		return;
	}

	if (mState == DispatcherState::ReaderWithCardRemoved)
	{
		Q_EMIT fireReceived(IfdMessageType::IFDStatus,
				QJsonDocument::fromJson("{\n"
										"    \"CardAvailable\": false,\n"
										"    \"ConnectedReader\": true,\n"
										"    \"ContextHandle\": \"TestContext\",\n"
										"    \"EFATR\": null,\n"
										"    \"EFDIR\": null,\n"
										"    \"MaxAPDULength\": 500,\n"
										"    \"PINPad\": false,\n"
										"    \"SlotName\": \"SlotName\",\n"
										"    \"msg\": \"IFDStatus\"\n"
										"}\n").object(), mId);

		Q_EMIT fireReceived(IfdMessageType::IFDStatus,
				QJsonDocument::fromJson("{\n"
										"    \"CardAvailable\": true,\n"
										"    \"ConnectedReader\": true,\n"
										"    \"ContextHandle\": \"TestContext\",\n"
										"    \"EFATR\": null,\n"
										"    \"EFDIR\": null,\n"
										"    \"MaxAPDULength\": 500,\n"
										"    \"PINPad\": false,\n"
										"    \"SlotName\": \"SlotName\",\n"
										"    \"msg\": \"IFDStatus\"\n"
										"}\n").object(), mId);
	}

	if (pMessage->getType() == IfdMessageType::IFDEstablishContext)
	{
		bool withCard = (mState == DispatcherState::ReaderWithCard || mState == DispatcherState::ReaderWithCardError);
		ReaderInfo info(QStringLiteral("NFC Reader"), ReaderManagerPluginType::PCSC, CardInfo(withCard ? CardType::EID_CARD : CardType::NONE));
		const QSharedPointer<IfdMessage> message(new IfdStatus(info));
		Q_EMIT fireReceived(message->getType(), QJsonDocument::fromJson(message->toByteArray(IfdVersion::Version::v2, mContextHandle)).object(), mId);
		return;
	}

	ECardApiResult::Minor resultMinor = ECardApiResult::Minor::null;
	switch (mState)
	{
		case DispatcherState::ReaderWithCardError:
			resultMinor = ECardApiResult::Minor::AL_Unknown_Error;
			break;

		case DispatcherState::ReaderWithCardRemoved:
			resultMinor = ECardApiResult::Minor::IFDL_Terminal_NoCard;
			break;

		default:
			break;
	}

	if (pMessage->getType() == IfdMessageType::IFDConnect)
	{
		const QSharedPointer<const IfdConnect> request = pMessage.staticCast<const IfdConnect>();
		const QString readerName = request->getSlotName();
		const QSharedPointer<IfdMessage> message(new IfdConnectResponse(readerName, resultMinor));
		Q_EMIT fireReceived(message->getType(), QJsonDocument::fromJson(message->toByteArray(IfdVersion::Version::v2, mContextHandle)).object(), mId);
	}

	if (pMessage->getType() == IfdMessageType::IFDTransmit)
	{
		const QSharedPointer<const IfdTransmit> request = pMessage.staticCast<const IfdTransmit>();
		const QString readerName = request->getSlotHandle();
		const QSharedPointer<IfdMessage> message(new IfdTransmitResponse(readerName, resultMinor == ECardApiResult::Minor::null ? QByteArray("pong") : QByteArray(), resultMinor));
		Q_EMIT fireReceived(message->getType(), QJsonDocument::fromJson(message->toByteArray(IfdVersion::Version::v2, mContextHandle)).object(), mId);
	}

	if (pMessage->getType() == IfdMessageType::IFDDisconnect)
	{
		const QSharedPointer<const IfdDisconnect> request = pMessage.staticCast<const IfdDisconnect>();
		const QString readerName = request->getSlotHandle();
		const QSharedPointer<IfdMessage> message(new IfdDisconnectResponse(readerName, resultMinor));
		Q_EMIT fireReceived(message->getType(), QJsonDocument::fromJson(message->toByteArray(IfdVersion::Version::v2, mContextHandle)).object(), mId);
	}

	if (pMessage->getType() == IfdMessageType::IFDEstablishPACEChannel)
	{
		const QSharedPointer<const IfdEstablishPaceChannel> request = pMessage.staticCast<const IfdEstablishPaceChannel>();
		const QString readerName = request->getSlotHandle();
		const QSharedPointer<IfdMessage> message(new IfdEstablishPaceChannelResponse(readerName, EstablishPaceChannelOutput(CardReturnCode::OK), resultMinor));
		Q_EMIT fireReceived(message->getType(), QJsonDocument::fromJson(message->toByteArray(IfdVersion::Version::v2, mContextHandle)).object(), mId);
	}

	if (pMessage->getType() == IfdMessageType::IFDModifyPIN)
	{
		const QSharedPointer<const IfdModifyPin> request = pMessage.staticCast<const IfdModifyPin>();
		const QString readerName = request->getSlotHandle();
		const QSharedPointer<IfdMessage> message(new IfdModifyPinResponse(readerName, QByteArray(), resultMinor));
		Q_EMIT fireReceived(message->getType(), QJsonDocument::fromJson(message->toByteArray(IfdVersion::Version::v2, mContextHandle)).object(), mId);
	}
}


MockIfdDispatcher::DispatcherState MockIfdDispatcher::getState() const
{
	return mState;
}


void MockIfdDispatcher::setState(DispatcherState pState)
{
	mState = pState;
}


void MockIfdDispatcher::onClosed()
{
	Q_EMIT fireClosed(GlobalStatus::Code::No_Error, mId);
}


void MockIfdDispatcher::onReceived(const QSharedPointer<const IfdMessage>& pMessage)
{
	Q_EMIT fireReceived(pMessage->getType(), QJsonDocument::fromJson(pMessage->toByteArray(IfdVersion::Version::v2, mContextHandle)).object(), mId);
}

/*!
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#include "MockRemoteDispatcher.h"

#include "messages/IfdConnect.h"
#include "messages/IfdConnectResponse.h"
#include "messages/IfdDisconnect.h"
#include "messages/IfdDisconnectResponse.h"
#include "messages/IfdStatus.h"
#include "messages/IfdTransmit.h"
#include "messages/IfdTransmitResponse.h"

#include "MockDataChannel.h"

#include <QTest>
#include <QUuid>


using namespace governikus;


MockRemoteDispatcher::MockRemoteDispatcher(DispatcherState pState)
	: RemoteDispatcherClient(IfdVersion::Version::v_test, QSharedPointer<MockDataChannel>::create())
	, mState(pState)
	, mId(QUuid::createUuid().toString())
	, mContextHandle(QStringLiteral("#TestContext"))
{
}


QString MockRemoteDispatcher::getId() const
{
	return mId;
}


const QString& MockRemoteDispatcher::getContextHandle() const
{
	return mContextHandle;
}


void MockRemoteDispatcher::send(const QSharedPointer<const RemoteMessage>& pMessage)
{
	QVERIFY(pMessage);

	Q_EMIT fireSend(pMessage);

	if (pMessage->getType() == RemoteCardMessageType::IFDEstablishContext)
	{
		if (mState == DispatcherState::WithoutReader)
		{
			return;
		}

		bool withCard = (mState == DispatcherState::ReaderWithCard || mState == DispatcherState::ReaderWithCardError);
		const QSharedPointer<RemoteMessage> message(new IfdStatus(QStringLiteral("NFC Reader"), PaceCapabilities(false), 500, true, withCard));
		Q_EMIT fireReceived(message->getType(), QJsonDocument::fromJson(message->toByteArray(mContextHandle)).object(), mId);
		return;
	}

	const ECardApiResult::Minor resultMinor = (mState == DispatcherState::ReaderWithCardError ? ECardApiResult::Minor::AL_Unknown_Error : ECardApiResult::Minor::null);

	if (pMessage->getType() == RemoteCardMessageType::IFDConnect)
	{
		const QSharedPointer<const IfdConnect> request = pMessage.staticCast<const IfdConnect>();
		const QString readerName = request->getSlotName();
		const QSharedPointer<RemoteMessage> message(new IfdConnectResponse(readerName, resultMinor));
		Q_EMIT fireReceived(message->getType(), QJsonDocument::fromJson(message->toByteArray(mContextHandle)).object(), mId);
	}

	if (pMessage->getType() == RemoteCardMessageType::IFDTransmit)
	{
		const QSharedPointer<const IfdTransmit> request = pMessage.staticCast<const IfdTransmit>();
		const QString readerName = request->getSlotHandle();
		const QSharedPointer<RemoteMessage> message(new IfdTransmitResponse(readerName, resultMinor == ECardApiResult::Minor::null ? QByteArray("pong") : QByteArray(), resultMinor));
		Q_EMIT fireReceived(message->getType(), QJsonDocument::fromJson(message->toByteArray(mContextHandle)).object(), mId);
	}

	if (pMessage->getType() == RemoteCardMessageType::IFDDisconnect)
	{
		const QSharedPointer<const IfdDisconnect> request = pMessage.staticCast<const IfdDisconnect>();
		const QString readerName = request->getSlotHandle();
		const QSharedPointer<RemoteMessage> message(new IfdDisconnectResponse(readerName, resultMinor));
		Q_EMIT fireReceived(message->getType(), QJsonDocument::fromJson(message->toByteArray(mContextHandle)).object(), mId);
	}
}


MockRemoteDispatcher::DispatcherState MockRemoteDispatcher::getState() const
{
	return mState;
}


void MockRemoteDispatcher::setState(DispatcherState pState)
{
	mState = pState;
}


void MockRemoteDispatcher::onClosed()
{
	Q_EMIT fireClosed(GlobalStatus::Code::No_Error, mId);
}


void MockRemoteDispatcher::onReceived(const QSharedPointer<const RemoteMessage>& pMessage)
{
	Q_EMIT fireReceived(pMessage->getType(), QJsonDocument::fromJson(pMessage->toByteArray(mContextHandle)).object(), mId);
}

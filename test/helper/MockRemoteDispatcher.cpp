/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#include "MockRemoteDispatcher.h"

#include "messages/IfdConnect.h"
#include "messages/IfdConnectResponse.h"
#include "messages/IfdDisconnect.h"
#include "messages/IfdDisconnectResponse.h"
#include "messages/IfdStatus.h"
#include "messages/IfdTransmit.h"
#include "messages/IfdTransmitResponse.h"
#include "MockIfdStatus.h"

#include <QTest>


using namespace governikus;


MockRemoteDispatcher::MockRemoteDispatcher(DispatcherState pState)
	: mState(pState)
	, mId()
	, mContextHandle()
{
}


const QString& MockRemoteDispatcher::getId() const
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
		const QSharedPointer<RemoteMessage> message(new MockIfdStatus(QStringLiteral("NFC Reader"), PaceCapabilities(false), 500, true, withCard));
		Q_EMIT fireReceived(message, sharedFromThis());
		return;
	}

	const QString errorMsg = mState == DispatcherState::ReaderWithCardError ? QStringLiteral("Error requested by the test") : QString();

	if (pMessage->getType() == RemoteCardMessageType::IFDConnect)
	{
		const QSharedPointer<const IfdConnect> request = pMessage.dynamicCast<const IfdConnect>();
		const QString readerName = request->getSlotName();
		const QSharedPointer<RemoteMessage> message(new IfdConnectResponse(readerName, errorMsg));
		Q_EMIT fireReceived(message, sharedFromThis());
	}

	if (pMessage->getType() == RemoteCardMessageType::IFDTransmit)
	{
		const QSharedPointer<const IfdTransmit> request = pMessage.dynamicCast<const IfdTransmit>();
		const QString readerName = request->getSlotHandle();
		const QSharedPointer<RemoteMessage> message(new IfdTransmitResponse(readerName, errorMsg.isEmpty() ? QByteArray("pong") : QByteArray(), errorMsg));
		Q_EMIT fireReceived(message, sharedFromThis());
	}

	if (pMessage->getType() == RemoteCardMessageType::IFDDisconnect)
	{
		const QSharedPointer<const IfdDisconnect> request = pMessage.dynamicCast<const IfdDisconnect>();
		const QString readerName = request->getSlotHandle();
		const QSharedPointer<RemoteMessage> message(new IfdDisconnectResponse(readerName, errorMsg));
		Q_EMIT fireReceived(message, sharedFromThis());
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
	Q_EMIT fireClosed(GlobalStatus::Code::RemoteReader_CloseCode_NormalClose, sharedFromThis());
}


void MockRemoteDispatcher::onReceived(const QSharedPointer<const RemoteMessage>& pMessage)
{
	Q_EMIT fireReceived(pMessage, sharedFromThis());
}

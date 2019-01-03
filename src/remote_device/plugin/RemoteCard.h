/*!
 * \brief Implementation of \ref Card for remote reader.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Card.h"
#include "messages/RemoteMessage.h"
#include "RemoteDispatcherClient.h"

#include <QMutex>
#include <QSharedPointer>
#include <QString>
#include <QWaitCondition>


namespace governikus
{

class RemoteCard
	: public Card
{
	Q_OBJECT

	private:
		bool mWaitingForAnswer;
		QWaitCondition mWaitCondition;
		QMutex mResponseAvailable, mProcessResponse;

		RemoteCardMessageType mExpectedAnswerType;
		QJsonObject mResponse;
		const QSharedPointer<RemoteDispatcherClient> mRemoteDispatcher;
		QString mReaderName;
		QString mSlotHandle;
		bool mConnected;

		bool sendMessage(const QSharedPointer<const RemoteMessage>& pMessage, RemoteCardMessageType pExpectedAnswer, unsigned long pTimeout);

	private Q_SLOTS:
		void onMessageReceived(RemoteCardMessageType pMessageTpe, const QJsonObject pJsonObject);
		void onDispatcherClosed(GlobalStatus::Code pCloseCode, const QString& pId);

	Q_SIGNALS:
		void fireCardRemoved();

	public:
		RemoteCard(const QSharedPointer<RemoteDispatcherClient>& pRemoteDispatcher, const QString& pReaderName);
		virtual ~RemoteCard() override;

		virtual CardReturnCode connect() override;
		virtual CardReturnCode disconnect() override;
		virtual bool isConnected() override;

		virtual CardReturnCode transmit(const CommandApdu& pCmd, ResponseApdu& pRes) override;

		virtual CardReturnCode establishPaceChannel(PacePasswordId pPasswordId, const QByteArray& pChat, const QByteArray& pCertificateDescription, EstablishPaceChannelOutput& pChannelOutput, quint8 pTimeoutSeconds = 60) override;

		virtual CardReturnCode setEidPin(quint8 pTimeoutSeconds, ResponseApdu& pResponseApdu) override;
};

} // namespace governikus

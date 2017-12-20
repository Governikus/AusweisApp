/*!
 * \brief Implementation of \ref Card for remote reader.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Card.h"
#include "messages/RemoteMessage.h"
#include "RemoteDispatcher.h"

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
		QSharedPointer<const RemoteMessage> mResponse;
		const QSharedPointer<RemoteDispatcher> mRemoteDispatcher;
		QString mReaderName;
		bool mConnected;

		bool sendMessage(const QSharedPointer<const RemoteMessage>& pMessage, RemoteCardMessageType pExpectedAnswer, unsigned long pTimeout);

	private Q_SLOTS:
		void onMessageReceived(const QSharedPointer<const RemoteMessage>& pMessage);
		void onDispatcherClosed(GlobalStatus::Code pCloseCode, const QSharedPointer<RemoteDispatcher>& pRemoteDispatcher);

	Q_SIGNALS:
		void fireCardRemoved();

	public:
		RemoteCard(const QSharedPointer<RemoteDispatcher>& pRemoteDispatcher, const QString& pReaderName);
		virtual ~RemoteCard() override;

		virtual CardReturnCode connect() override;
		virtual CardReturnCode disconnect() override;
		virtual bool isConnected() override;

		virtual CardReturnCode transmit(const CommandApdu& pCmd, ResponseApdu& pRes) override;

		virtual CardReturnCode establishPaceChannel(PACE_PASSWORD_ID pPasswordId, const QByteArray& pChat, const QByteArray& pCertificateDescription, EstablishPACEChannelOutput& pChannelOutput, quint8 pTimeoutSeconds = 60) override;

};

} /* namespace governikus */

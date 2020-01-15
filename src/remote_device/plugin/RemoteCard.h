/*!
 * \brief Implementation of \ref Card for smartphone as card reader (SaC).
 *
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
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
		void onMessageReceived(RemoteCardMessageType pMessageTpe, const QJsonObject& pJsonObject);
		void onDispatcherClosed(GlobalStatus::Code pCloseCode, const QString& pId);

	Q_SIGNALS:
		void fireCardRemoved();

	public:
		RemoteCard(const QSharedPointer<RemoteDispatcherClient>& pRemoteDispatcher, const QString& pReaderName);
		virtual ~RemoteCard() override;

		virtual CardReturnCode connect() override;
		virtual CardReturnCode disconnect() override;
		virtual bool isConnected() override;

		virtual ResponseApduResult transmit(const CommandApdu& pCmd) override;

		virtual EstablishPaceChannelOutput establishPaceChannel(PacePasswordId pPasswordId, const QByteArray& pChat, const QByteArray& pCertificateDescription, quint8 pTimeoutSeconds = 60) override;

		virtual ResponseApduResult setEidPin(quint8 pTimeoutSeconds) override;
};

} // namespace governikus

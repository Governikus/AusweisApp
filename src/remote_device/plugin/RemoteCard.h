/*!
 * \brief Implementation of \ref Card for smartphone as card reader (SaC).
 *
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
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
		QString mProgressMessage;

		bool sendMessage(const QSharedPointer<const RemoteMessage>& pMessage, RemoteCardMessageType pExpectedAnswer, unsigned long pTimeout);

	private Q_SLOTS:
		void onMessageReceived(RemoteCardMessageType pMessageTpe, const QJsonObject& pJsonObject);
		void onDispatcherClosed(GlobalStatus::Code pCloseCode, const QString& pId);

	Q_SIGNALS:
		void fireCardRemoved();

	public:
		RemoteCard(const QSharedPointer<RemoteDispatcherClient>& pRemoteDispatcher, const QString& pReaderName);
		~RemoteCard() override;

		CardReturnCode connect() override;
		CardReturnCode disconnect() override;
		bool isConnected() override;
		void setProgressMessage(const QString& pMessage, int pProgress = -1) override;

		ResponseApduResult transmit(const CommandApdu& pCmd) override;

		EstablishPaceChannelOutput establishPaceChannel(PacePasswordId pPasswordId, int pPreferredPinLength, const QByteArray& pChat, const QByteArray& pCertificateDescription, quint8 pTimeoutSeconds = 60) override;

		ResponseApduResult setEidPin(quint8 pTimeoutSeconds) override;
};

} // namespace governikus

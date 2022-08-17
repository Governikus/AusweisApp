/*!
 * \brief Implementation of \ref Card for smartphone as card reader (SaC).
 *
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Card.h"
#include "IfdDispatcherClient.h"
#include "messages/IfdMessage.h"

#include <QMutex>
#include <QSharedPointer>
#include <QString>
#include <QWaitCondition>


namespace governikus
{

class IfdCard
	: public Card
{
	Q_OBJECT

	private:
		bool mWaitingForAnswer;
		QWaitCondition mWaitCondition;
		QMutex mResponseAvailable;
		QMutex mProcessResponse;

		IfdMessageType mExpectedAnswerType;
		QJsonObject mResponse;
		const QSharedPointer<IfdDispatcherClient> mDispatcher;
		QString mReaderName;
		QString mSlotHandle;
		bool mConnected;
		QString mProgressMessage;

		bool sendMessage(const QSharedPointer<const IfdMessage>& pMessage, IfdMessageType pExpectedAnswer, unsigned long pTimeout);

	private Q_SLOTS:
		void onMessageReceived(IfdMessageType pMessageTpe, const QJsonObject& pJsonObject);
		void onDispatcherClosed(GlobalStatus::Code pCloseCode, const QString& pId);

	Q_SIGNALS:
		void fireCardRemoved();

	public:
		IfdCard(const QSharedPointer<IfdDispatcherClient>& pDispatcher, const QString& pReaderName);
		~IfdCard() override;

		CardReturnCode establishConnection() override;
		CardReturnCode releaseConnection() override;
		bool isConnected() const override;
		void setProgressMessage(const QString& pMessage, int pProgress = -1) override;

		ResponseApduResult transmit(const CommandApdu& pCmd) override;

		EstablishPaceChannelOutput establishPaceChannel(PacePasswordId pPasswordId, int pPreferredPinLength, const QByteArray& pChat, const QByteArray& pCertificateDescription, quint8 pTimeoutSeconds = 60) override;

		ResponseApduResult setEidPin(quint8 pTimeoutSeconds) override;
};

} // namespace governikus

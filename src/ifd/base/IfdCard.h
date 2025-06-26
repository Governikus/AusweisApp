/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Card.h"
#include "IfdDispatcherClient.h"
#include "messages/IfdMessage.h"

#include <QMutex>
#include <QSharedPointer>
#include <QString>
#include <QWaitCondition>


class test_IfdCard;


namespace governikus
{

class IfdCard
	: public Card
{
	Q_OBJECT

	friend class ::test_IfdCard;

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
		bool mCardRemoved;

		bool sendMessage(const QSharedPointer<const IfdMessage>& pMessage, IfdMessageType pExpectedAnswer, unsigned long pExtraTimeout = 0);

	private Q_SLOTS:
		void onMessageReceived(IfdMessageType pMessageType, const QJsonObject& pJsonObject);
		void onDispatcherClosed(GlobalStatus::Code pCloseCode, const QByteArray& pId);

	Q_SIGNALS:
		void fireCardRemoved();

	public:
		IfdCard(const QSharedPointer<IfdDispatcherClient>& pDispatcher, const QString& pReaderName);
		~IfdCard() override;

		CardReturnCode establishConnection() override;
		CardReturnCode releaseConnection() override;
		bool isConnected() const override;
		void setProgressMessage(const QString& pMessage, int pProgress = -1) override;
		void setErrorMessage(const QString& pMessage) override;

		ResponseApduResult transmit(const CommandApdu& pCmd) override;

		EstablishPaceChannelOutput establishPaceChannel(PacePasswordId pPasswordId, int pPreferredPinLength, const QByteArray& pChat, const QByteArray& pCertificateDescription) override;

		CardReturnCode destroyPaceChannel() override;

		ResponseApduResult setEidPin(quint8 pTimeoutSeconds) override;
};

} // namespace governikus

/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Implementation of card object for PC/SC
 *  *
 */

#pragma once

#include "Card.h"
#include "CardReturnCode.h"
#include "PcscReader.h"
#include "PcscUtils.h"

#include <QByteArray>
#include <QPointer>
#include <QTimer>


namespace governikus
{

class PcscCard
	: public Card
{
	Q_OBJECT

	private:
		struct CardResult
		{
			PCSC_RETURNCODE mReturnCode;
			QByteArray mResponse = QByteArray();
		};

		QPointer<PcscReader> mReader;
		PCSC_INT mProtocol;
		SCARDCONTEXT mContextHandle;
		SCARDHANDLE mCardHandle;
		QTimer mTimer;

		CardResult transmit(const QByteArray& pSendBuffer);
		CardResult transmit(const QByteArray& pSendBuffer, const SCARD_IO_REQUEST* pSendPci);
		CardResult control(PCSC_INT pCntrCode, const QByteArray& pCntrInput);

	private Q_SLOTS:
		void sendSCardStatus();

	public:
		explicit PcscCard(PcscReader* pPcscReader);
		~PcscCard() override;

		CardReturnCode establishConnection() override;
		CardReturnCode releaseConnection() override;
		bool isConnected() const override;

		ResponseApduResult transmit(const CommandApdu& pCmd) override;

		EstablishPaceChannelOutput establishPaceChannel(PacePasswordId pPasswordId, int pPreferredPinLength, const QByteArray& pChat, const QByteArray& pCertificateDescription) override;

		CardReturnCode destroyPaceChannel() override;

		ResponseApduResult setEidPin(quint8 pTimeoutSeconds) override;
};

} // namespace governikus

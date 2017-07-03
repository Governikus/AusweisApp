/*!
 * PcscCard.h
 *
 * \brief Implementation of card object for PC/SC
 *  *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "Card.h"
#include "PcscReader.h"
#include "PcscUtils.h"
#include "ReturnCode.h"

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
		QPointer<PcscReader> mReader;
		PCSC_INT mProtocol;
		SCARDCONTEXT mContextHandle;
		PCSC_CARDHANDLE mCardHandle;
		QTimer mTimer;

		PCSC_RETURNCODE transmit(const QByteArray& pSendBuffer, QByteArray& pReceiveBuffer);

		PCSC_RETURNCODE transmit(const QByteArray& pSendBuffer,
				QByteArray& pReceiveBuffer,
				const SCARD_IO_REQUEST* pSendPci,
				SCARD_IO_REQUEST& pRecvPci,
				PCSC_INT& pBytesReceived);

		PCSC_RETURNCODE control(PCSC_INT pCntrCode, const QByteArray& pCntrInput, QByteArray& pCntrOutput);

	private Q_SLOTS:
		void sendSCardStatus();

	public:
		PcscCard(PcscReader* pPcscReader);
		virtual ~PcscCard();

		virtual ReturnCode connect() override;
		virtual ReturnCode disconnect() override;
		virtual bool isConnected() override;

		virtual ReturnCode transmit(const CommandApdu& pCmd, ResponseApdu& pRes) override;

		virtual ReturnCode establishPaceChannel(PACE_PIN_ID pPinId, const QByteArray& pChat, const QByteArray& pCertificateDescription, EstablishPACEChannelOutput& pChannelOutput, int pTimeoutSeconds) override;

		virtual ReturnCode destroyPaceChannel() override;

		virtual ReturnCode setEidPin(unsigned int pTimeoutSeconds) override;
};

} /* namespace governikus */

/*!
 * \brief Implementation of card object for PC/SC
 *  *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
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
		virtual ~PcscCard() override;

		virtual CardReturnCode connect() override;
		virtual CardReturnCode disconnect() override;
		virtual bool isConnected() override;

		virtual ResponseApduResult transmit(const CommandApdu& pCmd) override;

		virtual EstablishPaceChannelOutput establishPaceChannel(PacePasswordId pPasswordId, const QByteArray& pChat, const QByteArray& pCertificateDescription, quint8 pTimeoutSeconds) override;

		virtual CardReturnCode destroyPaceChannel() override;

		virtual ResponseApduResult setEidPin(quint8 pTimeoutSeconds) override;
};

} // namespace governikus

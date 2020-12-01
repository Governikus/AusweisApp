/*!
 * \brief CardConnectionWorker mock for tests
 *
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "CardConnectionWorker.h"
#include "MockReader.h"

#include <QByteArrayList>

namespace governikus
{

class MockCardConnectionWorker
	: public CardConnectionWorker
{
	Q_OBJECT

	private:
		QPointer<Reader> mReader;
		QList<CardReturnCode> mResponseCodes;
		QByteArrayList mResponseData;
		QList<CardReturnCode> mPaceCodes;

		ResponseApduResult getMockedResponse();

	public:
		explicit MockCardConnectionWorker(Reader* pReader = new MockReader());
		virtual ~MockCardConnectionWorker() override;

		void addResponse(CardReturnCode pCode, const QByteArray& pData = QByteArray());
		void addPaceCode(CardReturnCode pCode);

		virtual ResponseApduResult transmit(const CommandApdu& pCommandApdu) override;
		virtual CardReturnCode updateRetryCounter() override;
		virtual EstablishPaceChannelOutput establishPaceChannel(PacePasswordId pPasswordId,
				const QByteArray& pPasswordValue,
				const QByteArray& pChat,
				const QByteArray& pCertificateDescription) override;
		virtual CardReturnCode destroyPaceChannel() override;
		virtual ResponseApduResult setEidPin(const QByteArray& pNewPin, quint8 pTimeoutSeconds) override;
};

} // namespace governikus

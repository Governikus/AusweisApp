/*!
 * \brief CardConnectionWorker mock for tests
 *
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
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

	public:
		explicit MockCardConnectionWorker(Reader* pReader = new MockReader());
		virtual ~MockCardConnectionWorker() override;

		void addResponse(CardReturnCode pCode, const QByteArray& pData = QByteArray());
		void addPaceCode(CardReturnCode pCode);

		virtual CardReturnCode transmit(const CommandApdu& pCommandApdu, ResponseApdu& pResponseApdu) override;
		virtual CardReturnCode updateRetryCounter() override;
		virtual CardReturnCode establishPaceChannel(PacePasswordId pPasswordId,
				const QString& pPasswordValue,
				const QByteArray& pChat,
				const QByteArray& pCertificateDescription,
				EstablishPaceChannelOutput& pChannelOutput) override;
		virtual CardReturnCode destroyPaceChannel() override;
		virtual CardReturnCode setEidPin(const QString& pNewPin, quint8 pTimeoutSeconds, ResponseApdu& pResponseApdu) override;
};

} // namespace governikus

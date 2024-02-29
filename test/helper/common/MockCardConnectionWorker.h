/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief CardConnectionWorker mock for tests
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
		QList<CommandApdu> mCommands;
		QList<CardReturnCode> mResponseCodes;
		QByteArrayList mResponseData;
		QList<CardReturnCode> mPaceCodes;

		ResponseApduResult getMockedResponse();

	public:
		explicit MockCardConnectionWorker(Reader* pReader = new MockReader());
		~MockCardConnectionWorker() override;

		const QList<CommandApdu>& getCommands() const;
		void addResponse(CardReturnCode pCode, const QByteArray& pData = QByteArray());
		void addPaceCode(CardReturnCode pCode);

		CardReturnCode readFile(const FileRef& pFileRef, QByteArray& pFileContent, int pLe = CommandApdu::SHORT_MAX_LE) override;
		ResponseApduResult transmit(const CommandApdu& pCommandApdu) override;
		CardReturnCode updateRetryCounter() override;
		EstablishPaceChannelOutput establishPaceChannel(PacePasswordId pPasswordId,
				const QByteArray& pPasswordValue,
				const QByteArray& pChat,
				const QByteArray& pCertificateDescription) override;
		CardReturnCode destroyPaceChannel() override;
		ResponseApduResult setEidPin(const QByteArray& pNewPin, quint8 pTimeoutSeconds) override;
};

} // namespace governikus

/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "CardConnectionWorker.h"
#include "MockReader.h"
#include "TestHookThread.h"

#include <QByteArrayList>
#include <QSharedPointer>


namespace governikus
{

class MockCardConnectionWorker
	: public CardConnectionWorker
{
	Q_OBJECT

	private:
		QList<CommandApdu> mCommands;
		QList<CardReturnCode> mResponseCodes;
		QByteArrayList mResponseData;
		QList<CardReturnCode> mPaceCodes;

		ResponseApduResult getMockedResponse();
		explicit MockCardConnectionWorker(Reader* pReader);

	public:
		static QSharedPointer<MockCardConnectionWorker> create(Reader* pReader = new MockReader());
		static QSharedPointer<MockCardConnectionWorker> create(TestHookThread* pThread, Reader* pReader = new MockReader());
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

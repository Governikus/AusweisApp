/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

#include "MockCardConnectionWorker.h"

#include <QThread>


using namespace governikus;


MockCardConnectionWorker::MockCardConnectionWorker(Reader* pReader)
	: CardConnectionWorker(pReader)
	, mResponseCodes()
	, mResponseData()
	, mPaceCodes()
{
	pReader->setParent(this);
}


QSharedPointer<MockCardConnectionWorker> MockCardConnectionWorker::create(Reader* pReader)
{
	return QSharedPointer<MockCardConnectionWorker>(new MockCardConnectionWorker(pReader));
}


QSharedPointer<MockCardConnectionWorker> MockCardConnectionWorker::create(TestHookThread* pThread, Reader* pReader)
{
	if (pThread == nullptr)
	{
		return create(pReader);
	}

	const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(pReader), &MockCardConnectionWorker::deleteLater);
	worker->moveToThread(pThread->getThread());
	pThread->waitForDestruction(worker.data());
	return worker;
}


MockCardConnectionWorker::~MockCardConnectionWorker()
{
	Q_ASSERT(QThread::currentThread() == thread());
}


const QList<CommandApdu>& MockCardConnectionWorker::getCommands() const
{
	return mCommands;
}


void MockCardConnectionWorker::addResponse(CardReturnCode pCode, const QByteArray& pData)
{
	mResponseCodes += pCode;
	mResponseData += pData;
}


void MockCardConnectionWorker::addPaceCode(CardReturnCode pCode)
{
	mPaceCodes += pCode;
}


ResponseApduResult MockCardConnectionWorker::getMockedResponse()
{
	if (mResponseData.empty())
	{
		return {CardReturnCode::UNDEFINED};
	}

	return {mResponseCodes.takeFirst(), ResponseApdu(mResponseData.takeFirst())};
}


CardReturnCode MockCardConnectionWorker::readFile(const FileRef& pFileRef, QByteArray& pFileContent, int pLe)
{
	Q_UNUSED(pFileRef)
	Q_UNUSED(pLe)

	const auto& [returnCode, result] = getMockedResponse();
	pFileContent += result.getData();

	return returnCode;

}


ResponseApduResult MockCardConnectionWorker::transmit(const CommandApdu& pCommandApdu)
{
	mCommands += pCommandApdu;

	return getMockedResponse();
}


CardReturnCode MockCardConnectionWorker::updateRetryCounter()
{
	if (!getReaderInfo().hasCard())
	{
		return CardReturnCode::CARD_NOT_FOUND;
	}
	return CardReturnCode::OK;
}


EstablishPaceChannelOutput MockCardConnectionWorker::establishPaceChannel(PacePasswordId pPasswordId,
		const QByteArray& pPasswordValue,
		const QByteArray& pChat,
		const QByteArray& pCertificateDescription)
{
	Q_UNUSED(pPasswordId)
	Q_UNUSED(pPasswordValue)
	Q_UNUSED(pChat)
	Q_UNUSED(pCertificateDescription)
	return EstablishPaceChannelOutput(mPaceCodes.empty() ? CardReturnCode::UNDEFINED : mPaceCodes.takeFirst());
}


CardReturnCode MockCardConnectionWorker::destroyPaceChannel()
{
	return mPaceCodes.empty() ? CardReturnCode::UNDEFINED : mPaceCodes.takeFirst();
}


ResponseApduResult MockCardConnectionWorker::setEidPin(const QByteArray& pNewPin, quint8 pTimeoutSeconds)
{
	Q_UNUSED(pNewPin)
	Q_UNUSED(pTimeoutSeconds)
	return getMockedResponse();
}

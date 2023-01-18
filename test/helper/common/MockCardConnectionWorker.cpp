/*!
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "MockCardConnectionWorker.h"

using namespace governikus;


MockCardConnectionWorker::MockCardConnectionWorker(Reader* pReader)
	: CardConnectionWorker(pReader)
	, mReader(pReader)
	, mResponseCodes()
	, mResponseData()
	, mPaceCodes()
{
}


MockCardConnectionWorker::~MockCardConnectionWorker()
{
	if (!mReader.isNull())
	{
		mReader->deleteLater();
	}
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
	if (!mReader->getReaderInfo().hasCard())
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

/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
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


ResponseApduResult MockCardConnectionWorker::transmit(const CommandApdu& pCommandApdu)
{
	Q_UNUSED(pCommandApdu)
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
		const QString& pPasswordValue,
		const QByteArray& pChat,
		const QByteArray& pCertificateDescription)
{
	Q_UNUSED(pPasswordId)
	Q_UNUSED(pPasswordValue)
	Q_UNUSED(pChat)
	Q_UNUSED(pCertificateDescription)
	return mPaceCodes.empty() ? CardReturnCode::UNDEFINED : mPaceCodes.takeFirst();
}


CardReturnCode MockCardConnectionWorker::destroyPaceChannel()
{
	return mPaceCodes.empty() ? CardReturnCode::UNDEFINED : mPaceCodes.takeFirst();
}


ResponseApduResult MockCardConnectionWorker::setEidPin(const QString& pNewPin, quint8 pTimeoutSeconds)
{
	Q_UNUSED(pNewPin)
	Q_UNUSED(pTimeoutSeconds)
	return getMockedResponse();
}

/*!
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
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


CardReturnCode MockCardConnectionWorker::transmit(const CommandApdu& pCommandApdu, ResponseApdu& pResponseApdu)
{
	Q_UNUSED(pCommandApdu);
	pResponseApdu.setBuffer(mResponseData.empty() ? QByteArray() : mResponseData.takeFirst());
	return mResponseCodes.empty() ? CardReturnCode::UNDEFINED : mResponseCodes.takeFirst();
}


CardReturnCode MockCardConnectionWorker::updateRetryCounter()
{
	if (!mReader->getReaderInfo().hasCard())
	{
		return CardReturnCode::CARD_NOT_FOUND;
	}
	return CardReturnCode::OK;
}


CardReturnCode MockCardConnectionWorker::establishPaceChannel(PacePasswordId pPasswordId,
		const QString& pPasswordValue,
		const QByteArray& pChat,
		const QByteArray& pCertificateDescription,
		EstablishPaceChannelOutput& pChannelOutput)
{
	Q_UNUSED(pPasswordId);
	Q_UNUSED(pPasswordValue);
	Q_UNUSED(pChat);
	Q_UNUSED(pCertificateDescription);
	Q_UNUSED(pChannelOutput);
	return mPaceCodes.empty() ? CardReturnCode::UNDEFINED : mPaceCodes.takeFirst();
}


CardReturnCode MockCardConnectionWorker::destroyPaceChannel()
{
	return mPaceCodes.empty() ? CardReturnCode::UNDEFINED : mPaceCodes.takeFirst();
}


CardReturnCode MockCardConnectionWorker::setEidPin(const QString& pNewPin, quint8 pTimeoutSeconds, ResponseApdu& pResponseApdu)
{
	Q_UNUSED(pNewPin);
	Q_UNUSED(pTimeoutSeconds);
	pResponseApdu.setBuffer(mResponseData.empty() ? QByteArray() : mResponseData.takeFirst());
	return mResponseCodes.empty() ? CardReturnCode::UNDEFINED : mResponseCodes.takeFirst();
}

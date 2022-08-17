/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#include "SetEidPinCommand.h"

using namespace governikus;


SetEidPinCommand::SetEidPinCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
		const QByteArray& pNewPin, quint8 pTimeoutSeconds)
	: BaseCardCommand(pCardConnectionWorker)
	, mNewPin(pNewPin)
	, mTimeoutSeconds(pTimeoutSeconds)
	, mResponseApdu()
{
}


void SetEidPinCommand::internalExecute()
{
	auto [returnCode, responseApdu] = getCardConnectionWorker()->setEidPin(mNewPin, mTimeoutSeconds);
	mReturnCode = returnCode;
	mResponseApdu = responseApdu;
}


const ResponseApdu& SetEidPinCommand::getResponseApdu() const
{
	return mResponseApdu;
}

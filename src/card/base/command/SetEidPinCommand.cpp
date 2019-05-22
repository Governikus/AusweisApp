/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include "SetEidPinCommand.h"

using namespace governikus;


SetEidPinCommand::SetEidPinCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
		const QString& pNewPin, quint8 pTimeoutSeconds)
	: BaseCardCommand(pCardConnectionWorker)
	, mNewPin(pNewPin)
	, mTimeoutSeconds(pTimeoutSeconds)
	, mResponseApdu()
{
}


void SetEidPinCommand::internalExecute()
{
	mReturnCode = mCardConnectionWorker->setEidPin(mNewPin, mTimeoutSeconds, mResponseApdu);
}


const ResponseApdu& SetEidPinCommand::getResponseApdu() const
{
	return mResponseApdu;
}

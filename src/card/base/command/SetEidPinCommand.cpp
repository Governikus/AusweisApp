/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "CardConnection.h"
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


SetEidPinCommand::~SetEidPinCommand()
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

/*!
 * SetEidPinCommand.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "CardConnection.h"
#include "SetEidPinCommand.h"


using namespace governikus;


SetEidPinCommand::SetEidPinCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
		const QString& pNewPin, unsigned int pTimeoutSeconds)
	: BaseCardCommand(pCardConnectionWorker)
	, mNewPin(pNewPin)
	, mTimeoutSeconds(pTimeoutSeconds)
{
}


SetEidPinCommand::~SetEidPinCommand()
{
}


void SetEidPinCommand::internalExecute()
{
	mReturnCode = mCardConnectionWorker->setEidPin(mNewPin, mTimeoutSeconds);
}

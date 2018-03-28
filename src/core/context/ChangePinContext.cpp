/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "ChangePinContext.h"

using namespace governikus;


ChangePinContext::ChangePinContext()
	: WorkflowContext()
	, mNewPin()
	, mSuccessMessage()
{
}


const QString& ChangePinContext::getNewPin() const
{
	return mNewPin;
}


void ChangePinContext::setNewPin(const QString& pNewPin)
{
	if (mNewPin != pNewPin)
	{
		mNewPin = pNewPin;
		Q_EMIT fireNewPinChanged();
	}
}


const QString& ChangePinContext::getSuccessMessage() const
{
	return mSuccessMessage;
}


void ChangePinContext::setSuccessMessage(const QString& pSuccessMessage)
{
	if (pSuccessMessage != mSuccessMessage)
	{
		mSuccessMessage = pSuccessMessage;
		Q_EMIT fireSuccessMessageChanged();
	}
}

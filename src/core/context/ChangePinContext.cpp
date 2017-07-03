/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "ChangePinContext.h"

using namespace governikus;


ChangePinContext::ChangePinContext()
	: WorkflowContext()
	, mNewPin()
	, mPuk()
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


const QString& ChangePinContext::getPuk() const
{
	return mPuk;
}


void ChangePinContext::setPuk(const QString& pPuk)
{
	if (mPuk != pPuk)
	{
		mPuk = pPuk;
		Q_EMIT firePukChanged();
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

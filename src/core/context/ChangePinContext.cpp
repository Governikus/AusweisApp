/*!
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#include "ChangePinContext.h"

using namespace governikus;


ChangePinContext::ChangePinContext(bool pRequestTransportPin)
	: WorkflowContext()
	, mNewPin()
	, mSuccessMessage()
	, mRequestTransportPin(pRequestTransportPin)
{
}


ChangePinContext::~ChangePinContext()
{
#ifndef QT_NO_DEBUG
	if (!QCoreApplication::applicationName().startsWith(QLatin1String("Test")))
	{
		Q_ASSERT(getNewPin().isEmpty() && "PACE passwords must be cleared as soon as possible.");
	}
#endif
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


void ChangePinContext::resetPacePasswords()
{
	setNewPin(QString());
	WorkflowContext::resetPacePasswords();
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


bool ChangePinContext::requestTransportPin() const
{
	return mRequestTransportPin;
}

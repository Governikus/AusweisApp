/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#include "ChangePinContext.h"

using namespace governikus;


ChangePinContext::ChangePinContext(bool pRequestTransportPin, bool pActivateUi)
	: WorkflowContext(Action::PIN, pActivateUi)
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


bool ChangePinContext::isRequestTransportPin() const
{
	return mRequestTransportPin;
}


QVector<AcceptedEidType> ChangePinContext::getAcceptedEidTypes() const
{
	if (mRequestTransportPin)
	{
		return {AcceptedEidType::CARD_CERTIFIED};
	}

	return {AcceptedEidType::CARD_CERTIFIED, AcceptedEidType::SE_CERTIFIED, AcceptedEidType::SE_ENDORSED, AcceptedEidType::HW_KEYSTORE};
}

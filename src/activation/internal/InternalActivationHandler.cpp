/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "InternalActivationHandler.h"

#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(activation)

InternalActivationHandler::InternalActivationHandler()
	: ActivationHandler()
{
}


InternalActivationHandler::~InternalActivationHandler()
{
}


void InternalActivationHandler::stop()
{
}


void InternalActivationHandler::runAuthentication(InternalActivationContext* pContext)
{
	Q_EMIT fireAuthenticationRequest(pContext);
}


bool InternalActivationHandler::start()
{
	return true;
}

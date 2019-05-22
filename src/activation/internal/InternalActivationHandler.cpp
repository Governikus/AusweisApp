/*!
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
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


void InternalActivationHandler::runAuthentication(const QSharedPointer<ActivationContext>& pContext)
{
	Q_EMIT fireAuthenticationRequest(pContext);
}


bool InternalActivationHandler::start()
{
	return true;
}

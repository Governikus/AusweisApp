/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "ActivationHandler.h"

#include "ActivationContext.h"
#include "Initializer.h"

using namespace governikus;

INIT_FUNCTION([] {
			qRegisterMetaType<UiModule>("UiModule");
			qRegisterMetaType<QSharedPointer<ActivationContext> >("QSharedPointer<ActivationContext>");
		})


void ActivationHandler::onApplicationActivated()
{
}

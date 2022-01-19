/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "ActivationHandler.h"

#include "ActivationContext.h"
#include "Initializer.h"

#include <QUrlQuery>


using namespace governikus;


INIT_FUNCTION([] {
			qRegisterMetaType<UiModule>("UiModule");
			qRegisterMetaType<QSharedPointer<ActivationContext> >("QSharedPointer<ActivationContext>");
		})


ActivationHandler::ActivationRequest ActivationHandler::getRequest(const QUrl& pUrl)
{
	const auto queryItems = QUrlQuery(pUrl).queryItems();
	for (auto& item : queryItems)
	{
		ActivationType type = Enum<ActivationType>::fromString(item.first.toUpper(), ActivationType::UNKNOWN);
		if (type != ActivationType::UNKNOWN)
		{
			return {type, item.second};
		}
	}

	return {ActivationType::UNKNOWN, QString()};
}


void ActivationHandler::onApplicationActivated()
{
}

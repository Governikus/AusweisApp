/*!
 * CustomSchemeActivationHandler.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "CustomSchemeActivationHandler.h"

#include "CustomSchemeActivationContext.h"

#include <QDesktopServices>
#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(activation)

CustomSchemeActivationHandler::CustomSchemeActivationHandler()
{
}


CustomSchemeActivationHandler::~CustomSchemeActivationHandler()
{
}


bool CustomSchemeActivationHandler::start()
{
	QDesktopServices::setUrlHandler(QStringLiteral("eid"), this, "onCustomUrl");
	return true;
}


void CustomSchemeActivationHandler::stop()
{
	QDesktopServices::unsetUrlHandler(QStringLiteral("eid"));
}


void CustomSchemeActivationHandler::onCustomUrl(const QUrl& pUrl)
{
	qCDebug(activation) << "Got new request";
	qCDebug(activation) << "Request URL:" << pUrl;

	if (pUrl.port() != 24727 || (pUrl.host() != QLatin1String("127.0.0.1") && pUrl.host() != QLatin1String("localhost")))
	{
		qCWarning(activation) << "Request type: unknown";
		return;
	}

	if (pUrl.path() == QLatin1String("/eID-Client"))
	{
		const auto urlParameter = getQueryParameter(pUrl);

		if (urlParameter.contains(QStringLiteral("showui")))
		{
			qCDebug(activation) << "Request type: showui";
			UiModule module = Enum<UiModule>::fromString(urlParameter.value(QStringLiteral("showui")).toUpper(), UiModule::DEFAULT);
			Q_EMIT fireShowUiRequest(module);
			return;
		}
		else if (urlParameter.contains(QStringLiteral("tctokenurl")))
		{
			qCDebug(activation) << "Request type: authentication";
			Q_EMIT fireAuthenticationRequest(new CustomSchemeActivationContext(pUrl));
			return;
		}
	}

	qCWarning(activation) << "Request type: unknown";
}

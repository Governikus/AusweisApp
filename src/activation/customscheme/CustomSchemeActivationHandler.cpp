/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "CustomSchemeActivationHandler.h"

#include "CustomSchemeActivationContext.h"

#include <QDesktopServices>
#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(activation)


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

	if (pUrl.port() != 24727 ||
			(pUrl.host() != QLatin1String("127.0.0.1") && pUrl.host() != QLatin1String("localhost")) ||
			(pUrl.path() != QLatin1String("/eID-Client")))
	{
		qCWarning(activation) << "Request type: unknown";
		return;
	}

	qCDebug(activation) << "Request type: authentication";
	const auto& context = QSharedPointer<CustomSchemeActivationContext>::create(pUrl);
	connect(context.data(), &CustomSchemeActivationContext::fireShowUserInformation, this, &ActivationHandler::fireShowUserInformation);
	Q_EMIT fireAuthenticationRequest(context);
}

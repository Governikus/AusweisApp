/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
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
	QDesktopServices::setUrlHandler(QStringLiteral("https"), this, "onCustomUrl");
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

	const bool isLocalhost = (pUrl.host() == QLatin1String("127.0.0.1") || pUrl.host() == QLatin1String("localhost")) && pUrl.port() == 24727;
	const bool isUniversal = pUrl.host() == QLatin1String("www.ausweisapp.bund.de");
	if (pUrl.path() != QLatin1String("/eID-Client") || !(isLocalhost || isUniversal))
	{
		if (pUrl.scheme() == QLatin1String("https"))
		{
			QDesktopServices::openUrl(pUrl);
		}
		else
		{
			qCWarning(activation) << "Request type: unknown";
		}
		return;
	}

	qCDebug(activation) << "Request type: authentication";
	const auto& context = QSharedPointer<CustomSchemeActivationContext>::create(pUrl);
	connect(context.data(), &CustomSchemeActivationContext::fireShowUserInformation, this, &ActivationHandler::fireShowUserInformation);
	Q_EMIT fireAuthenticationRequest(context);
}

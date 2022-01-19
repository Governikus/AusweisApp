/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "CustomSchemeActivationHandler.h"

#include "CustomSchemeActivationContext.h"

#include <QDesktopServices>
#include <QLoggingCategory>

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniObject>
#endif

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(activation)


bool CustomSchemeActivationHandler::start()
{
	QDesktopServices::setUrlHandler(QStringLiteral("eid"), this, "onCustomUrl");
	QDesktopServices::setUrlHandler(QStringLiteral("http"), this, "onCustomUrl");
	QDesktopServices::setUrlHandler(QStringLiteral("https"), this, "onCustomUrl");
	return true;
}


void CustomSchemeActivationHandler::stop()
{
	QDesktopServices::unsetUrlHandler(QStringLiteral("eid"));
	QDesktopServices::unsetUrlHandler(QStringLiteral("http"));
	QDesktopServices::unsetUrlHandler(QStringLiteral("https"));
}


void CustomSchemeActivationHandler::onCustomUrl(const QUrl& pUrl)
{
	QString referrer;
#ifdef Q_OS_ANDROID
	referrer = QAndroidJniObject::callStaticObjectMethod<jstring>("com/governikus/ausweisapp2/MainActivity", "fetchStoredReferrer").toString();
	qCDebug(activation) << "Got new request by:" << referrer;
#else
	qCDebug(activation) << "Got new request";
#endif
	qCDebug(activation) << "Request URL:" << pUrl;

	const bool isLocalhost = (pUrl.host() == QLatin1String("127.0.0.1") || pUrl.host() == QLatin1String("localhost")) && pUrl.port() == 24727;
	const bool isUniversal = pUrl.host() == QLatin1String("www.ausweisapp.bund.de");
	if (pUrl.path() != QLatin1String("/eID-Client") || !(isLocalhost || isUniversal))
	{
		if (pUrl.scheme() == QLatin1String("https"))
		{
			qCDebug(activation) << "Not our business. Using the appropriate mechanism for the user's desktop environment.";
			QDesktopServices::openUrl(pUrl);
		}
		else
		{
			qCWarning(activation) << "Request type: unknown";
		}
		return;
	}

	const auto& [type, value] = getRequest(pUrl);
	switch (type)
	{
		case ActivationType::SHOWUI:
		{
			qCDebug(activation) << "Request type: showui";
			const UiModule module = Enum<UiModule>::fromString(value.toUpper(), UiModule::DEFAULT);
			Q_EMIT fireShowUiRequest(module);
			return;
		}

		case ActivationType::TCTOKENURL:
		{
			qCDebug(activation) << "Request type: authentication";
			const auto& context = QSharedPointer<CustomSchemeActivationContext>::create(pUrl, referrer);
			connect(context.data(), &CustomSchemeActivationContext::fireShowUserInformation, this, &ActivationHandler::fireShowUserInformation);
			Q_EMIT fireAuthenticationRequest(context);
			return;
		}

		default:
			qCWarning(activation) << "Unknown query type:" << pUrl;
	}
}

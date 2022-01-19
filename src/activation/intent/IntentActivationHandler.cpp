/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#include "IntentActivationHandler.h"

#include "IntentActivationContext.h"

#include <QLoggingCategory>

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniObject>
#endif


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(activation)


void IntentActivationHandler::onIntent(const QUrl& pUrl, const QString& pReferrer)
{
	qCDebug(activation) << "Got new request by:" << pReferrer;
	qCDebug(activation) << "Request URL:" << pUrl;

	if (pUrl.path() != QLatin1String("/eID-Client"))
	{
		qCWarning(activation) << "Request type: unknown";
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
			const auto& context = QSharedPointer<IntentActivationContext>::create(pUrl, pReferrer);
			connect(context.data(), &IntentActivationContext::fireShowUserInformation, this, &ActivationHandler::fireShowUserInformation);
			Q_EMIT fireAuthenticationRequest(context);
			return;
		}

		default:
			qCWarning(activation) << "Unknown query type:" << pUrl;
	}
}


bool IntentActivationHandler::start()
{
#ifdef Q_OS_ANDROID
	return true;

#else
	qCCritical(activation) << "Cannot use IntentActivationHandler on non android system";
	return false;

#endif
}


void IntentActivationHandler::stop()
{
}


void IntentActivationHandler::onApplicationActivated()
{
#ifdef Q_OS_ANDROID
	const QString& intent = QAndroidJniObject::callStaticObjectMethod<jstring>("com/governikus/ausweisapp2/MainActivity", "fetchStoredIntent").toString();
	const QString& referrer = QAndroidJniObject::callStaticObjectMethod<jstring>("com/governikus/ausweisapp2/MainActivity", "fetchStoredReferrer").toString();
	if (!intent.isNull())
	{
		onIntent(intent, referrer);
	}
#endif
}

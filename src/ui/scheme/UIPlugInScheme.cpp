/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInScheme.h"

#include "CustomSchemeActivationContext.h"
#include "UrlUtil.h"
#include "controller/AuthController.h"

#include <QDesktopServices>
#include <QLoggingCategory>

#ifdef Q_OS_ANDROID
	#include <QJniObject>
#endif

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(scheme)


UIPlugInScheme::UIPlugInScheme()
	: UIPlugIn()
{
	QDesktopServices::setUrlHandler(QStringLiteral("eid"), this, "onCustomUrl");
	QDesktopServices::setUrlHandler(QStringLiteral("http"), this, "onCustomUrl");
	QDesktopServices::setUrlHandler(QStringLiteral("https"), this, "onCustomUrl");
}


UIPlugInScheme::~UIPlugInScheme()
{
	QDesktopServices::unsetUrlHandler(QStringLiteral("eid"));
	QDesktopServices::unsetUrlHandler(QStringLiteral("http"));
	QDesktopServices::unsetUrlHandler(QStringLiteral("https"));
}


void UIPlugInScheme::onCustomUrl(const QUrl& pUrl)
{
	QString referrer;
#ifdef Q_OS_ANDROID
	referrer = QJniObject::callStaticObjectMethod<jstring>("com/governikus/ausweisapp2/MainActivity", "fetchStoredReferrer").toString();
	qCDebug(scheme) << "Got new request by:" << referrer;
#else
	qCDebug(scheme) << "Got new request";
#endif
	qCDebug(scheme) << "Request URL:" << pUrl;

	const bool isLocalhost = (pUrl.host() == QLatin1String("127.0.0.1") || pUrl.host() == QLatin1String("localhost")) && pUrl.port() == 24727;
	const bool isUniversal = pUrl.host() == QLatin1String("www.ausweisapp.bund.de");
	if (pUrl.path() != QLatin1String("/eID-Client") || !(isLocalhost || isUniversal))
	{
		if (pUrl.scheme() == QLatin1String("https"))
		{
			qCDebug(scheme) << "Not our business. Using the appropriate mechanism for the user's desktop environment.";
			QDesktopServices::openUrl(pUrl);
		}
		else
		{
			qCWarning(scheme) << "Request type: unknown";
		}
		return;
	}

	const auto queryUrl = QUrlQuery(pUrl);
	UrlUtil::setHiddenSettings(queryUrl);
	const auto& [type, value] = UrlUtil::getRequest(queryUrl);
	switch (type)
	{
		case UrlQueryRequest::SHOWUI:
		{
			qCDebug(scheme) << "Request type: showui";
			const UiModule showModule = Enum<UiModule>::fromString(value.toUpper(), UiModule::DEFAULT);
			Q_EMIT fireShowUiRequested(showModule);
			return;
		}

		case UrlQueryRequest::TCTOKENURL:
		{
			qCDebug(scheme) << "Request type: authentication";
			const auto& context = QSharedPointer<CustomSchemeActivationContext>::create(pUrl, referrer);
			connect(context.data(), &CustomSchemeActivationContext::fireShowUserInformation, this, &UIPlugIn::fireShowUserInformationRequested);
			Q_EMIT fireWorkflowRequested(AuthController::createWorkflowRequest(context));
			return;
		}

		default:
			qCWarning(scheme) << "Unknown query type:" << pUrl;
	}
}


void UIPlugInScheme::doShutdown()
{
}


void UIPlugInScheme::onWorkflowStarted(QSharedPointer<WorkflowContext> pContext)
{
	Q_UNUSED(pContext)
}


void UIPlugInScheme::onWorkflowFinished(QSharedPointer<WorkflowContext> pContext)
{
	Q_UNUSED(pContext)
}

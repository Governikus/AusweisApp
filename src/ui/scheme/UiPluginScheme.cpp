/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginScheme.h"

#include "UrlUtil.h"
#include "controller/AuthController.h"

#include <QDesktopServices>
#include <QLoggingCategory>

#ifdef Q_OS_ANDROID
	#include <QJniObject>
#endif


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(scheme)


UiPluginScheme::UiPluginScheme()
	: UiPlugin()
{
	QDesktopServices::setUrlHandler(QStringLiteral("eid"), this, "onCustomUrl");
	QDesktopServices::setUrlHandler(QStringLiteral("http"), this, "onCustomUrl");
	QDesktopServices::setUrlHandler(QStringLiteral("https"), this, "onCustomUrl");
}


UiPluginScheme::~UiPluginScheme()
{
	QDesktopServices::unsetUrlHandler(QStringLiteral("eid"));
	QDesktopServices::unsetUrlHandler(QStringLiteral("http"));
	QDesktopServices::unsetUrlHandler(QStringLiteral("https"));
}


void UiPluginScheme::onCustomUrl(const QUrl& pUrl)
{
	Referrer referrer;
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
		if (pUrl.scheme().startsWith(QLatin1String("http")))
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
			const UiModule showModule = UrlUtil::prepareToEnum(value, UiModule::DEFAULT);
			Q_EMIT fireShowUiRequested(showModule);
			return;
		}

		case UrlQueryRequest::TCTOKENURL:
		{
			qCDebug(scheme) << "Request type: authentication";
			Q_EMIT fireWorkflowRequested(AuthController::createWorkflowRequest(pUrl, QVariant::fromValue(referrer)));
			return;
		}

		default:
			qCWarning(scheme) << "Unknown query type:" << pUrl;
	}
}


void UiPluginScheme::sendRedirect(const Referrer& pReferrer, const QUrl& pRedirectAddress) const
{
	if (!pRedirectAddress.isValid())
	{
		qCDebug(scheme) << "Invalid redirect URL:" << pRedirectAddress;
		return;
	}

	qCDebug(scheme) << "Perform redirect to URL" << pRedirectAddress;
#ifdef Q_OS_ANDROID
	QJniObject context = QNativeInterface::QAndroidApplication::context();
	if (context.callMethod<jboolean>("openUrl", "(Ljava/lang/String;Ljava/lang/String;)Z", QJniObject::fromString(pRedirectAddress.url()).object<jstring>(), QJniObject::fromString(pReferrer).object<jstring>()))
	{
		return;
	}
#else
	Q_UNUSED(pReferrer)
#endif
	QDesktopServices::openUrl(pRedirectAddress);
}


void UiPluginScheme::doShutdown()
{
}


void UiPluginScheme::onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest)
{
	Q_UNUSED(pRequest)
}


void UiPluginScheme::onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest)
{
	if (pRequest->getData().metaType() != QMetaType::fromType<Referrer>())
	{
		return;
	}

	if (const auto& context = pRequest->getContext().objectCast<AuthContext>(); context)
	{
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
		// Only skip redirects on mobile platforms because it induces a forced focus change
		if (context->isSkipMobileRedirect())
		{
			qCDebug(scheme) << "Skipping redirect, Workflow pending";
			return;
		}
#endif

		if (const auto& url = context->getRefreshUrl(); url.isValid())
		{
			/*
			 * Opening an URL on iOS will bring Safari to the foreground, our app
			 * will go to the background, causing the authentication controller to stop.
			 *
			 * Therefore we open the URL after workflow finished.
			 */
			const auto& referrer = pRequest->getData().value<Referrer>();
			QMetaObject::invokeMethod(this, [this, referrer, url] {
					sendRedirect(referrer, url);
				}, Qt::QueuedConnection);
		}
	}
}


void UiPluginScheme::onWorkflowUnhandled(const QSharedPointer<WorkflowRequest>& pRequest)
{
	if (pRequest->getData().metaType() == QMetaType::fromType<Referrer>()) // workflow is started by this plugin
	{
		Q_EMIT fireShowUserInformationRequested(GlobalStatus(GlobalStatus::Code::Workflow_AlreadyInProgress_Error).toErrorDescription());
	}
}

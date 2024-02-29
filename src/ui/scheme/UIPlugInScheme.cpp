/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInScheme.h"

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
			handleTcTokenUrl(pUrl, referrer);
			return;
		}

		default:
			qCWarning(scheme) << "Unknown query type:" << pUrl;
	}
}


void UIPlugInScheme::handleTcTokenUrl(const QUrl& pActivationUrl, const QString& pReferrer)
{
	const RedirectHandler handler = [pReferrer](const QUrl& pRefreshUrl, const GlobalStatus& pStatus){
				const auto& redirectAddress = UrlUtil::addMajorMinor(pRefreshUrl, pStatus);
				qCDebug(scheme) << "Determined redirect URL" << redirectAddress;

				if (!redirectAddress.isEmpty())
				{
					qCDebug(scheme) << "Perform redirect to URL" << redirectAddress;
#ifdef Q_OS_ANDROID
					QJniObject context = QNativeInterface::QAndroidApplication::context();
					if (context.callMethod<jboolean>("openUrl", "(Ljava/lang/String;Ljava/lang/String;)Z", QJniObject::fromString(redirectAddress.url()).object<jstring>(), QJniObject::fromString(pReferrer).object<jstring>()))
					{
						return;
					}
#else
					Q_UNUSED(pReferrer)
#endif
					QDesktopServices::openUrl(redirectAddress);
				}
			};

	Q_EMIT fireWorkflowRequested(AuthController::createWorkflowRequest(pActivationUrl, QVariant::fromValue(handler)));
}


void UIPlugInScheme::doShutdown()
{
}


void UIPlugInScheme::onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest)
{
	Q_UNUSED(pRequest)
}


void UIPlugInScheme::onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest)
{
	if (const auto& handler = pRequest->getData().value<RedirectHandler>(); handler)
	{
		if (const auto& context = pRequest->getContext().objectCast<AuthContext>(); context)
		{
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
			// Only skip redirects on mobile platforms because it induces a forced focus change
			if (context->isSkipMobileRedirect())
			{
				qDebug() << "Skipping redirect, Workflow pending";
				return;
			}
#endif

			const auto& url = context->getRefreshUrl();
			if (!url.isEmpty())
			{
				const auto& status = context->getStatus();

				/*
				 * Opening an URL on iOS will bring Safari to the foreground, our app
				 * will go to the background, causing the authentication controller to stop.
				 *
				 * Therefore we open the URL after workflow finished.
				 */
				QMetaObject::invokeMethod(QCoreApplication::instance(), [handler, url, status] {
						handler(url, status);
					}, Qt::QueuedConnection);
			}
		}
	}
}


void UIPlugInScheme::onWorkflowUnhandled(const QSharedPointer<WorkflowRequest>& pRequest)
{
	if (pRequest->getData().value<RedirectHandler>()) // workflow is started by this plugin
	{
		Q_EMIT fireShowUserInformationRequested(GlobalStatus(GlobalStatus::Code::Workflow_AlreadyInProgress_Error).toErrorDescription());
	}
}

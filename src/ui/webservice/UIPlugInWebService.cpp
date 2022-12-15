/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInWebService.h"

#include "Env.h"
#include "HttpServerStatusParser.h"
#include "VersionInfo.h"
#include "VersionNumber.h"
#include "WebserviceActivationContext.h"
#include "controller/AuthController.h"

#include <QCoreApplication>
#include <QFile>
#include <QLoggingCategory>
#include <QUrlQuery>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(webservice)


UIPlugInWebService::UIPlugInWebService()
	: UIPlugIn()
	, mServer()
{
}


bool UIPlugInWebService::initialize()
{
	return (mServer && mServer->isListening()) || listening();
}


bool UIPlugInWebService::listening()
{
	mServer = Env::getShared<HttpServer>();

	if (mServer->isListening())
	{
		connect(mServer.data(), &HttpServer::fireNewHttpRequest, this, &UIPlugInWebService::onNewRequest);
		return true;
	}

	QString showMessage;
	const quint16 port = HttpServer::cPort;
	for (const auto& address : std::as_const(HttpServer::cAddresses))
	{
		HttpServerStatusParser parser(port, address);
		QString serverAppName = parser.request() ? parser.getVersionInfo().getName() : parser.getServerHeader();
		if (serverAppName.startsWith(VersionInfo::getInstance().getName()))
		{
			qCDebug(webservice) << "We are already started... calling ShowUI";
			HttpServerRequestor requestor;
			const auto& reply = requestor.getRequest(HttpServerRequestor::createUrl(QStringLiteral("ShowUI=") + UiModule::CURRENT, port, address));
			if (reply.isNull())
			{
				qCWarning(webservice) << "ShowUI request timed out";
				showMessage.clear();
			}
			else if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == HTTP_STATUS_BAD_GATEWAY)
			{
				qCDebug(webservice) << "Proxy is started... rebind!";
				mServer->rebind();
				if (mServer->isListening())
				{
					connect(mServer.data(), &HttpServer::fireNewHttpRequest, this, &UIPlugInWebService::onNewRequest);
					return true;
				}
				else
				{
					showMessage = tr("Reverse-Proxy of AusweisApp2 is started and this instance cannot rebind port. Please ask your administrator!");
				}
			}
			else
			{
				qCDebug(webservice) << "ShowUI request succeeded";
				showMessage.clear();
			}

			break;
		}
		else
		{
			qCCritical(webservice) << "Cannot start application. Port on address is probably already bound by another program:" << serverAppName;

			if (showMessage.isEmpty())
			{
				//: ERROR ALL_PLATFORMS An unknown programme is using the local port on which the AA2 listens.
				showMessage = tr("An unknown program uses the required port (%1). Please exit the other program and try again!").arg(port);
			}

			if (!serverAppName.isEmpty())
			{
				//: ERROR ALL_PLATFORMS A known programme is using the local port on which the AA2 listens.
				showMessage = tr("The program (%1) uses the required port (%2). Please close %1 and try again!").arg(serverAppName).arg(port);
			}
		}
	}

	if (!showMessage.isEmpty())
	{
		Q_EMIT fireShowUserInformationRequested(showMessage);
	}

	mServer.reset();
	return false;
}


void UIPlugInWebService::onNewRequest(const QSharedPointer<HttpRequest>& pRequest)
{
	handle(pRequest);
}


void UIPlugInWebService::handleWorkflowRequest(const QSharedPointer<HttpRequest>& pRequest)
{
	Q_EMIT fireWorkflowRequested(AuthController::createWorkflowRequest(QSharedPointer<WebserviceActivationContext>::create(pRequest)));
}


void UIPlugInWebService::handleShowUiRequest(const QString& pUiModule, const QSharedPointer<HttpRequest>& pRequest)
{
	pRequest->send(HTTP_STATUS_OK);

	QString userAgent = QString::fromLatin1(pRequest->getHeader(QByteArrayLiteral("user-agent")));
	if (userAgent.startsWith(QCoreApplication::applicationName()))
	{
		QString version = userAgent.remove(QCoreApplication::applicationName() + QLatin1Char('/')).split(QLatin1Char(' ')).at(0);
		VersionNumber callerVersion(version);

		if (callerVersion > VersionNumber::getApplicationVersion())
		{
			qCWarning(webservice) << "Current version is lower than caller version";
			//: ERROR ALL_PLATFORMS The external request to show the UI requested a newer version than the one currently installed.
			Q_EMIT fireShowUserInformationRequested(tr("You tried to start a newer version (%1) of currently running AusweisApp2. Please stop the current version (%2) and start again!").arg(version, QCoreApplication::applicationVersion()));
			return;
		}
		else if (callerVersion < VersionNumber::getApplicationVersion())
		{
			qCWarning(webservice) << "Current version is higher than caller version";
			//: ERROR ALL_PLATFORMS The external request to show the UI requested an older version than the one currently installed.
			Q_EMIT fireShowUserInformationRequested(tr("You tried to start an older version (%1) of currently running AusweisApp2. Please open the currently running version (%2)!").arg(version, QCoreApplication::applicationVersion()));
			return;
		}
	}

	Q_EMIT fireShowUiRequested(Enum<UiModule>::fromString(pUiModule, UiModule::DEFAULT));
}


void UIPlugInWebService::onWorkflowStarted(QSharedPointer<WorkflowContext> pContext)
{
	Q_UNUSED(pContext)
}


void UIPlugInWebService::onWorkflowFinished(QSharedPointer<WorkflowContext> pContext)
{
	Q_UNUSED(pContext)
}


void UIPlugInWebService::doShutdown()
{
}

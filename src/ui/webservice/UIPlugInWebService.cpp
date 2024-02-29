/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInWebService.h"

#include "Env.h"
#include "HttpServerStatusParser.h"
#include "LanguageLoader.h"
#include "Template.h"
#include "UrlUtil.h"
#include "VersionInfo.h"
#include "VersionNumber.h"
#include "context/AuthContext.h"
#include "controller/AuthController.h"

#include <QCoreApplication>
#include <QFile>
#include <QLoggingCategory>
#include <QUrlQuery>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(webservice)


UIPlugInWebService::UIPlugInWebService()
	: UIPlugIn()
	, HttpHandler()
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
		const QString serverAppName = parser.request() ? parser.getVersionInfo().getImplementationTitle() : parser.getServerHeader();
		if (serverAppName.startsWith(VersionInfo::getInstance().getImplementationTitle()))
		{
			switch (handleExistingApp(port, address))
			{
				case ExistingAppResult::REBIND_SUCCEED:
					return true;

				case ExistingAppResult::REBIND_FAILED:
					showMessage = tr("Reverse-Proxy of %1 is started and this instance cannot rebind port. Please ask your administrator!").arg(QCoreApplication::applicationName());
					break;

				case ExistingAppResult::SHOWUI_SUCCEED:
				case ExistingAppResult::SHOWUI_TIMEOUT:
					showMessage.clear();
					break;

			}

			break;
		}

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

	if (!showMessage.isEmpty())
	{
		Q_EMIT fireShowUserInformationRequested(showMessage);
	}

	mServer.reset();
	return false;
}


UIPlugInWebService::ExistingAppResult UIPlugInWebService::handleExistingApp(quint16 pPort, const QHostAddress& pHost) const
{
	qCDebug(webservice) << "We are already started... calling ShowUI";
	HttpServerRequestor requestor;
	const auto& reply = requestor.getRequest(HttpServerRequestor::createUrl(QStringLiteral("ShowUI=") + UiModule::CURRENT, pPort, pHost));
	if (reply.isNull())
	{
		qCWarning(webservice) << "ShowUI request timed out";
		return ExistingAppResult::SHOWUI_TIMEOUT;
	}

	if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == HTTP_STATUS_BAD_GATEWAY)
	{
		qCDebug(webservice) << "Proxy is started... rebind!";
		mServer->rebind();
		if (mServer->isListening())
		{
			connect(mServer.data(), &HttpServer::fireNewHttpRequest, this, &UIPlugInWebService::onNewRequest);
			return ExistingAppResult::REBIND_SUCCEED;
		}

		return ExistingAppResult::REBIND_FAILED;
	}

	qCDebug(webservice) << "ShowUI request succeeded";
	return ExistingAppResult::SHOWUI_SUCCEED;
}


void UIPlugInWebService::onNewRequest(const QSharedPointer<HttpRequest>& pRequest)
{
	handle(pRequest);
}


void UIPlugInWebService::handleWorkflowRequest(const QSharedPointer<HttpRequest>& pRequest)
{
	const AuthContext::BrowserHandler handler = [this, pRequest](const QSharedPointer<AuthContext>& pContext){
				return sendRedirect(pRequest, pContext);
			};
	Q_EMIT fireWorkflowRequested(AuthController::createWorkflowRequest(pRequest->getUrl(), QVariant::fromValue(pRequest), handler));
}


void UIPlugInWebService::handleShowUiRequest(const QString& pUiModule, const QSharedPointer<HttpRequest>& pRequest)
{
	pRequest->send(HTTP_STATUS_OK);

	QString userAgent = QString::fromLatin1(pRequest->getHeader(QByteArrayLiteral("user-agent")));
	if (userAgent.startsWith(VersionInfo::getInstance().getImplementationTitle()))
	{
		QString version = userAgent.remove(VersionInfo::getInstance().getImplementationTitle() + QLatin1Char('/')).split(QLatin1Char(' ')).at(0);
		VersionNumber callerVersion(version);

		if (callerVersion > VersionNumber::getApplicationVersion())
		{
			qCWarning(webservice) << "Current version is lower than caller version";
			//: ERROR ALL_PLATFORMS The external request to show the UI requested a newer version than the one currently installed.
			Q_EMIT fireShowUserInformationRequested(tr("You tried to start a newer version (%1) of currently running %2. Please stop the current version (%3) and start again!").arg(version, QCoreApplication::applicationName(), QCoreApplication::applicationVersion()));
			return;
		}
		else if (callerVersion < VersionNumber::getApplicationVersion())
		{
			qCWarning(webservice) << "Current version is higher than caller version";
			//: ERROR ALL_PLATFORMS The external request to show the UI requested an older version than the one currently installed.
			Q_EMIT fireShowUserInformationRequested(tr("You tried to start an older version (%1) of currently running %2. Please open the currently running version (%3)!").arg(version, QCoreApplication::applicationName(), QCoreApplication::applicationVersion()));
			return;
		}
	}

	Q_EMIT fireShowUiRequested(UrlUtil::prepareToEnum(pUiModule, UiModule::DEFAULT));
}


void UIPlugInWebService::onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest)
{
	if (pRequest->getContext().objectCast<AuthContext>())
	{
		const auto& request = pRequest->getData().value<QSharedPointer<HttpRequest>>();
		if (request)
		{
			if (request->isConnected())
			{
				request->send(HTTP_STATUS_PROCESSING);
			}
			else
			{
				qCCritical(webservice) << "Cannot send 'Processing' to caller as connection is lost";
			}
		}
	}
}


void UIPlugInWebService::onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest)
{
	Q_UNUSED(pRequest)
}


void UIPlugInWebService::onWorkflowUnhandled(const QSharedPointer<WorkflowRequest>& pRequest)
{
	const auto& request = pRequest->getData().value<QSharedPointer<HttpRequest>>();
	if (request)
	{
		sendWorkflowAlreadyActive(request);
	}
}


void UIPlugInWebService::doShutdown()
{
	if (mServer)
	{
		mServer->disconnect(this);
		mServer.reset();
	}
}


void UIPlugInWebService::sendWorkflowAlreadyActive(const QSharedPointer<HttpRequest>& pRequest) const
{
	if (!pRequest || !pRequest->isConnected())
	{
		qCCritical(webservice) << "Cannot send to caller as connection is lost";
		return;
	}

	Template htmlTemplate = Template::fromFile(QStringLiteral(":/template.html"));
	//: ERROR ALL_PLATFORMS A new authentication request was received while the previous one was still running. Part of an HTML error page.
	htmlTemplate.setContextParameter(QStringLiteral("TITLE"), tr("Cannot start authentication"));
	htmlTemplate.setContextParameter(QStringLiteral("APPLICATION_LINK"), QStringLiteral("https://www.ausweisapp.bund.de/%1").arg(LanguageLoader::getLocaleCode()));
	//: ERROR ALL_PLATFORMS A new authentication request was received while the previous one was still running. Part of an HTML error page.
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER"), tr("Cannot start authentication"));
	//: ERROR ALL_PLATFORMS A new authentication request was received while the previous one was still running. Part of an HTML error page.
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER_EXPLANATION"), tr("An operation is already in progress."));
	//: ERROR ALL_PLATFORMS A new authentication request was received while the previous one was still running. Part of an HTML error page.
	htmlTemplate.setContextParameter(QStringLiteral("CONTENT_HEADER"), tr("Would you like to try again?"));
	htmlTemplate.setContextParameter(QStringLiteral("CONTENT_LINK"), pRequest->getUrl().toString());
	//: ERROR ALL_PLATFORMS A new authentication request was received while the previous one was still running. Part of an HTML error page.
	htmlTemplate.setContextParameter(QStringLiteral("CONTENT_BUTTON"), tr("Try again"));
	QByteArray htmlPage = htmlTemplate.render().toUtf8();

	HttpResponse response(HTTP_STATUS_CONFLICT);
	setCommonHeaders(response);
	response.setBody(htmlPage, QByteArrayLiteral("text/html; charset=utf-8"));
	pRequest->send(response);
}


QString UIPlugInWebService::sendErrorPage(const QSharedPointer<HttpRequest>& pRequest, http_status pStatusCode, const GlobalStatus& pStatus) const
{
	if (!pRequest->isConnected())
	{
		//: ERROR ALL_PLATFORMS No HTTP connection present.
		return tr("The browser connection was lost.");
	}

	qCDebug(webservice) << "Send error page to browser, error code" << pStatusCode;
	const auto& statusMsg = NetworkManager::getFormattedStatusMessage(pStatusCode);

	Template htmlTemplate = Template::fromFile(QStringLiteral(":/template.html"));
	htmlTemplate.setContextParameter(QStringLiteral("TITLE"), tr("Invalid request (%1)").arg(statusMsg));
	htmlTemplate.setContextParameter(QStringLiteral("APPLICATION_LINK"), QStringLiteral("https://www.ausweisapp.bund.de/%1").arg(LanguageLoader::getLocaleCode()));
	//: ERROR ALL_PLATFORMS Invalid request by the browser, part of an HTML error page
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER"), tr("Invalid request (%1)").arg(statusMsg));
	//: ERROR ALL_PLATFORMS Invalid request by the browser, part of an HTML error page
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER_EXPLANATION"), tr("Your browser sent a request that couldn't be interpreted."));
	//: ERROR ALL_PLATFORMS Invalid request by the browser, part of an HTML error page
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_SUBHEADER_LABEL"), tr("Error message:"));
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_SUBHEADER"), pStatus.toErrorDescription(true));
	//: ERROR ALL_PLATFORMS Invalid request by the browser, part of an HTML error page
	htmlTemplate.setContextParameter(QStringLiteral("CONTENT_HEADER"), tr("Would you like to report this error?"));
	htmlTemplate.setContextParameter(QStringLiteral("CONTENT_LINK"), QStringLiteral("https://www.ausweisapp.bund.de/%1/aa2/report").arg(LanguageLoader::getLocaleCode()));
	//: ERROR ALL_PLATFORMS Invalid request by the browser, part of an HTML error page
	htmlTemplate.setContextParameter(QStringLiteral("CONTENT_BUTTON"), tr("Report now"));
	QByteArray htmlPage = htmlTemplate.render().toUtf8();

	HttpResponse response(pStatusCode);
	setCommonHeaders(response);
	response.setBody(htmlPage, QByteArrayLiteral("text/html; charset=utf-8"));
	pRequest->send(response);
	return QString();
}


QString UIPlugInWebService::sendRedirect(const QSharedPointer<HttpRequest>& pRequest, const QSharedPointer<AuthContext>& pContext) const
{
	if (pContext->getStatus() == GlobalStatus::Code::Workflow_InternalError_BeforeTcToken)
	{
		return sendErrorPage(pRequest, HTTP_STATUS_INTERNAL_SERVER_ERROR, pContext->getStatus());
	}
	else if (pContext->isTcTokenNotFound())
	{
		return sendErrorPage(pRequest, HTTP_STATUS_NOT_FOUND, pContext->getStatus());
	}
	else if (pContext->getRefreshUrl().isEmpty())
	{
		if (pContext->getTcToken() != nullptr && pContext->getTcToken()->getCommunicationErrorAddress().isValid())
		{
			return sendRedirect(pRequest, pContext->getTcToken()->getCommunicationErrorAddress(), ECardApiResult(GlobalStatus::Code::Workflow_Communication_Missing_Redirect_Url));
		}
		return sendErrorPage(pRequest, HTTP_STATUS_BAD_REQUEST, pContext->getStatus());
	}

	const auto& result = pContext->getStartPaosResult().isOk() ? ECardApiResult(pContext->getStatus()) : pContext->getStartPaosResult();
	return sendRedirect(pRequest, pContext->getRefreshUrl(), result);
}


QString UIPlugInWebService::sendRedirect(const QSharedPointer<HttpRequest>& pRequest, const QUrl& pRedirectAddress, const ECardApiResult& pResult) const
{
	QUrl redirectAddressWithResult = UrlUtil::addMajorMinor(pRedirectAddress, GlobalStatus(pResult));
	qCDebug(webservice) << "Redirect URL:" << redirectAddressWithResult;

	if (!pRequest->isConnected())
	{
		const auto& url = QStringLiteral("<a href='%1'>%2</a>").arg(redirectAddressWithResult.toString(), redirectAddressWithResult.host());
		//: ERROR ALL_PLATFORMS The connection to the browser was lost/timed out..
		return tr("The connection to the browser was lost. No forwarding was executed. Please try to call the URL again manually: %1").arg(url);
	}

	HttpResponse response(HTTP_STATUS_SEE_OTHER);
	setCommonHeaders(response);
	response.setHeader(QByteArrayLiteral("Location"), redirectAddressWithResult.toEncoded());
	pRequest->send(response);
	return QString();
}


void UIPlugInWebService::setCommonHeaders(HttpResponse& pResponse) const
{
	pResponse.setHeader(QByteArrayLiteral("Connection"), QByteArrayLiteral("close"));
	pResponse.setHeader(QByteArrayLiteral("Cache-Control"), QByteArrayLiteral("no-cache, no-store"));
	pResponse.setHeader(QByteArrayLiteral("Pragma"), QByteArrayLiteral("no-cache"));
}

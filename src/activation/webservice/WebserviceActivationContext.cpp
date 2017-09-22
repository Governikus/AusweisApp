/*!
 * WebserviceActivationContext.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "WebserviceActivationContext.h"

#include "qhttpserver/qhttprequest.h"
#include "qhttpserver/qhttpresponse.h"
#include "Template.h"
#include "UrlUtil.h"

#include <QFile>
#include <QLoggingCategory>
#include <QUrlQuery>


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(activation)

void WebserviceActivationContext::setCommonHeaders(QSharedPointer<QHttpResponse> pResponse)
{
	WebserviceActivationContext::setServerHeader(*pResponse.data());
	pResponse->setHeader(QStringLiteral("Connection"), QStringLiteral("close"));
	pResponse->setHeader(QStringLiteral("Cache-Control"), QStringLiteral("no-cache, no-store"));
	pResponse->setHeader(QStringLiteral("Pragma"), QStringLiteral("no-cache"));
}


WebserviceActivationContext::WebserviceActivationContext(QSharedPointer<QHttpRequest> pRequest, QSharedPointer<QHttpResponse> pResponse)
	: ActivationContext()
	, mRequest(pRequest)
	, mResponse(pResponse)
{
}


WebserviceActivationContext::~WebserviceActivationContext()
{
}


QUrl WebserviceActivationContext::getActivationURL()
{
	return mRequest->url();
}


bool WebserviceActivationContext::sendProcessing()
{
	if (mResponse->getState() != QAbstractSocket::ConnectedState)
	{
		mSendError = tr("The browser connection was lost.");
		return false;
	}

	WebserviceActivationContext::setServerHeader(*mResponse.data());
	mResponse->setHeader(QStringLiteral("Content-Length"), QStringLiteral("0"));
	mResponse->writeHead(QHttpResponse::STATUS_PROCESSING);
	// allow final writing in StateRedirectBrowser
	mResponse->clearHeaders();
	return true;
}


bool WebserviceActivationContext::sendOperationAlreadyActive()
{
	if (mResponse->getState() != QAbstractSocket::ConnectedState)
	{
		mSendError = tr("The browser connection was lost.");
		return false;
	}

	Template htmlTemplate = Template::fromFile(QStringLiteral(":/html_templates/alreadyactive.html"));
	htmlTemplate.setContextParameter(QStringLiteral("TITLE"), tr("Cannot start authentication"));
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER"), tr("Cannot start authentication"));
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER_EXPLANATION"), tr("An operation is already in progress."));
	htmlTemplate.setContextParameter(QStringLiteral("CONTENT_HEADER"), tr("Would you like to try again?"));
	htmlTemplate.setContextParameter(QStringLiteral("CONTENT_LINK"), mRequest->url().toString());
	htmlTemplate.setContextParameter(QStringLiteral("CONTENT_BUTTON"), tr("Try again"));
	QByteArray htmlPage = htmlTemplate.render().toUtf8();

	WebserviceActivationContext::setServerHeader(*mResponse.data());
	mResponse->setHeader(QStringLiteral("Content-Type"), QStringLiteral("text/html; charset=utf-8"));
	mResponse->setHeader(QStringLiteral("Content-Length"), QString::number(htmlPage.size()));
	mResponse->writeHead(QHttpResponse::STATUS_NOT_FOUND);
	mResponse->end(htmlPage);
	return true;
}


bool WebserviceActivationContext::sendErrorPage(HttpStatusCode pStatusCode, const Result& pResult)
{
	if (mResponse->getState() != QAbstractSocket::ConnectedState)
	{
		mSendError = tr("The browser connection was lost.");
		return false;
	}

	qCDebug(activation) << "Send error page to browser, error code " << pStatusCode;
	Q_ASSERT(pStatusCode == HttpStatusCode::BAD_REQUEST || pStatusCode == HttpStatusCode::NOT_FOUND);
	QString statusCodeString;
	if (pStatusCode == HttpStatusCode::BAD_REQUEST)
	{
		statusCodeString = tr("400 Bad Request");
	}
	else if (pStatusCode == HttpStatusCode::NOT_FOUND)
	{
		statusCodeString = tr("404 Not found");
	}

	Template htmlTemplate = Template::fromFile(QStringLiteral(":/html_templates/error.html"));
	htmlTemplate.setContextParameter(QStringLiteral("TITLE"), statusCodeString);
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER"), tr("Invalid request"));
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER_EXPLANATION"), tr("Your browser sent a request that couldn't be interpreted."));
	htmlTemplate.setContextParameter(QStringLiteral("ERROR_MESSAGE_LABEL"), tr("Error message"));
	htmlTemplate.setContextParameter(QStringLiteral("ERROR_MESSAGE"), pResult.getMessage());
	htmlTemplate.setContextParameter(QStringLiteral("REPORT_HEADER"), tr("Would you like to report this error?"));
	htmlTemplate.setContextParameter(QStringLiteral("REPORT_LINK"), tr("https://www.ausweisapp.bund.de/en/feedback/melden-sie-einen-fehler/"));
	htmlTemplate.setContextParameter(QStringLiteral("REPORT_BUTTON"), tr("Report now"));
	QByteArray htmlPage = htmlTemplate.render().toUtf8();

	setCommonHeaders(mResponse);
	mResponse->setHeader(QStringLiteral("Content-Type"), QStringLiteral("text/html; charset=utf-8"));
	mResponse->setHeader(QStringLiteral("Content-Length"), QString::number(htmlPage.size()));
	mResponse->writeHead(static_cast<int>(pStatusCode));
	mResponse->end(htmlPage);
	return true;
}


bool WebserviceActivationContext::sendRedirect(const QUrl& pRedirectAddress, const Result& pResult)
{
	QUrl redirectAddressWithResult = UrlUtil::addMajorMinor(pRedirectAddress, pResult);
	qCDebug(activation) << "Redirect URL:" << redirectAddressWithResult;

	if (mResponse->getState() != QAbstractSocket::ConnectedState)
	{
		mSendError = tr("The connection to the bowser was lost. No forwarding was executed. Please try to"
						" call the URL again manually: <a href='%1'>%2</a>").arg(redirectAddressWithResult.toString(), redirectAddressWithResult.host());
		return false;
	}

	qCDebug(activation) << "Redirecting now to URL:" << redirectAddressWithResult;
	setCommonHeaders(mResponse);
	mResponse->setHeader(QStringLiteral("Location"), QString::fromLatin1(redirectAddressWithResult.toEncoded()));
	mResponse->setHeader(QStringLiteral("Content-Length"), QStringLiteral("0"));
	mResponse->writeHead(QHttpResponse::STATUS_SEE_OTHER);
	mResponse->end();
	return true;
}


void WebserviceActivationContext::setServerHeader(QHttpResponse& pReply)
{
	/*
	 * According to TR-03124-1, chapter 2.2.2.1, the Server-header shall have the following form:
	 */
	pReply.setHeader(QStringLiteral("Server"), QCoreApplication::applicationName() + "/" + QCoreApplication::applicationVersion() + " (TR-03124-1/1.2)");
}

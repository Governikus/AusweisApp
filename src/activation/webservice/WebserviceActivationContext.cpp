/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "WebserviceActivationContext.h"

#include "Template.h"
#include "UrlUtil.h"

#include <QFile>
#include <QLoggingCategory>
#include <QUrlQuery>


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(activation)

void WebserviceActivationContext::setCommonHeaders(HttpResponse& pResponse)
{
	pResponse.setHeader(QByteArrayLiteral("Connection"), QByteArrayLiteral("close"));
	pResponse.setHeader(QByteArrayLiteral("Cache-Control"), QByteArrayLiteral("no-cache, no-store"));
	pResponse.setHeader(QByteArrayLiteral("Pragma"), QByteArrayLiteral("no-cache"));
}


WebserviceActivationContext::WebserviceActivationContext(const QSharedPointer<HttpRequest>& pRequest)
	: ActivationContext()
	, mRequest(pRequest)
{
}


WebserviceActivationContext::~WebserviceActivationContext()
{
}


QUrl WebserviceActivationContext::getActivationURL() const
{
	return mRequest->getUrl();
}


bool WebserviceActivationContext::sendProcessing()
{
	if (!mRequest->isConnected())
	{
		mSendError = tr("The browser connection was lost.");
		return false;
	}

	mRequest->send(HttpStatusCode::PROCESSING);
	return true;
}


bool WebserviceActivationContext::sendOperationAlreadyActive()
{
	if (!mRequest->isConnected())
	{
		mSendError = tr("The browser connection was lost.");
		return false;
	}

	Template htmlTemplate = Template::fromFile(QStringLiteral(":/html_templates/alreadyactive.html"));
	htmlTemplate.setContextParameter(QStringLiteral("TITLE"), tr("Cannot start authentication"));
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER"), tr("Cannot start authentication"));
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER_EXPLANATION"), tr("An operation is already in progress."));
	htmlTemplate.setContextParameter(QStringLiteral("CONTENT_HEADER"), tr("Would you like to try again?"));
	htmlTemplate.setContextParameter(QStringLiteral("CONTENT_LINK"), mRequest->getUrl().toString());
	htmlTemplate.setContextParameter(QStringLiteral("CONTENT_BUTTON"), tr("Try again"));
	QByteArray htmlPage = htmlTemplate.render().toUtf8();

	HttpResponse response;
	response.setStatus(HttpStatusCode::NOT_FOUND);
	response.setBody(htmlPage, QByteArrayLiteral("text/html; charset=utf-8"));
	mRequest->send(response);
	return true;
}


bool WebserviceActivationContext::sendErrorPage(HttpStatusCode pStatusCode, const GlobalStatus& pStatus)
{
	if (!mRequest->isConnected())
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
	htmlTemplate.setContextParameter(QStringLiteral("ERROR_MESSAGE"), pStatus.toErrorDescription(true));
	htmlTemplate.setContextParameter(QStringLiteral("REPORT_HEADER"), tr("Would you like to report this error?"));
	htmlTemplate.setContextParameter(QStringLiteral("REPORT_LINK"), tr("https://www.ausweisapp.bund.de/en/feedback/melden-sie-einen-fehler/"));
	htmlTemplate.setContextParameter(QStringLiteral("REPORT_BUTTON"), tr("Report now"));
	QByteArray htmlPage = htmlTemplate.render().toUtf8();

	HttpResponse response;
	setCommonHeaders(response);
	response.setStatus(pStatusCode);
	response.setBody(htmlPage, QByteArrayLiteral("text/html; charset=utf-8"));
	mRequest->send(response);
	return true;
}


bool WebserviceActivationContext::sendRedirect(const QUrl& pRedirectAddress, const GlobalStatus& pStatus)
{
	QUrl redirectAddressWithResult = UrlUtil::addMajorMinor(pRedirectAddress, pStatus);
	qCDebug(activation) << "Redirect URL:" << redirectAddressWithResult;

	if (!mRequest->isConnected())
	{
		mSendError = tr("The connection to the bowser was lost. No forwarding was executed. Please try to"
						" call the URL again manually: <a href='%1'>%2</a>").arg(redirectAddressWithResult.toString(), redirectAddressWithResult.host());
		return false;
	}

	qCDebug(activation) << "Redirecting now to URL:" << redirectAddressWithResult;
	HttpResponse response;
	setCommonHeaders(response);
	response.setHeader(QByteArrayLiteral("Location"), redirectAddressWithResult.toEncoded());
	response.setStatus(HttpStatusCode::SEE_OTHER);
	mRequest->send(response);
	return true;
}

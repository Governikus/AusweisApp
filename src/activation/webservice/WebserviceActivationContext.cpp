/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
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


QUrl WebserviceActivationContext::getActivationURL() const
{
	return mRequest->getUrl();
}


bool WebserviceActivationContext::sendProcessing()
{
	if (!mRequest->isConnected())
	{
		//: ERROR ALL_PLATFORMS No HTTP connection present.
		mSendError = tr("The browser connection was lost.");
		return false;
	}

	mRequest->send(HTTP_STATUS_PROCESSING);
	return true;
}


bool WebserviceActivationContext::sendOperationAlreadyActive()
{
	if (!mRequest->isConnected())
	{
		//: ERROR ALL_PLATFORMS No HTTP connection present.
		mSendError = tr("The browser connection was lost.");
		return false;
	}

	Template htmlTemplate = Template::fromFile(QStringLiteral(":/html_templates/alreadyactive.html"));
	//: ERROR ALL_PLATFORMS A new authentication request was received while the previous one was still running. Part of an HTML error page.
	htmlTemplate.setContextParameter(QStringLiteral("TITLE"), tr("Cannot start authentication"));
	//: ERROR ALL_PLATFORMS A new authentication request was received while the previous one was still running. Part of an HTML error page.
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER"), tr("Cannot start authentication"));
	//: ERROR ALL_PLATFORMS A new authentication request was received while the previous one was still running. Part of an HTML error page.
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER_EXPLANATION"), tr("An operation is already in progress."));
	//: ERROR ALL_PLATFORMS A new authentication request was received while the previous one was still running. Part of an HTML error page.
	htmlTemplate.setContextParameter(QStringLiteral("CONTENT_HEADER"), tr("Would you like to try again?"));
	htmlTemplate.setContextParameter(QStringLiteral("CONTENT_LINK"), mRequest->getUrl().toString());
	//: ERROR ALL_PLATFORMS A new authentication request was received while the previous one was still running. Part of an HTML error page.
	htmlTemplate.setContextParameter(QStringLiteral("CONTENT_BUTTON"), tr("Try again"));
	QByteArray htmlPage = htmlTemplate.render().toUtf8();

	HttpResponse response;
	response.setStatus(HTTP_STATUS_CONFLICT);
	response.setBody(htmlPage, QByteArrayLiteral("text/html; charset=utf-8"));
	mRequest->send(response);
	return true;
}


bool WebserviceActivationContext::sendErrorPage(http_status pStatusCode, const GlobalStatus& pStatus)
{
	if (!mRequest->isConnected())
	{
		//: ERROR ALL_PLATFORMS No HTTP connection present.
		mSendError = tr("The browser connection was lost.");
		return false;
	}

	qCDebug(activation) << "Send error page to browser, error code" << pStatusCode;
	Q_ASSERT(pStatusCode == HTTP_STATUS_BAD_REQUEST || pStatusCode == HTTP_STATUS_NOT_FOUND);
	QString statusCodeString;
	if (pStatusCode == HTTP_STATUS_BAD_REQUEST)
	{
		//: ERROR ALL_PLATFORMS HTTP error code 400, invalid request, part of an HTML error page.
		statusCodeString = tr("400 Bad Request");
	}
	else if (pStatusCode == HTTP_STATUS_NOT_FOUND)
	{
		//: ERROR ALL_PLATFORMS HTTP error code 404, invalid request, part of an HTML error page.
		statusCodeString = tr("404 Not found");
	}

	Template htmlTemplate = Template::fromFile(QStringLiteral(":/html_templates/error.html"));
	htmlTemplate.setContextParameter(QStringLiteral("TITLE"), statusCodeString);
	//: ERROR ALL_PLATFORMS Invalid request by the browser, part of an HTML error page
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER"), tr("Invalid request"));
	//: ERROR ALL_PLATFORMS Invalid request by the browser, part of an HTML error page
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER_EXPLANATION"), tr("Your browser sent a request that couldn't be interpreted."));
	//: ERROR ALL_PLATFORMS Invalid request by the browser, part of an HTML error page
	htmlTemplate.setContextParameter(QStringLiteral("ERROR_MESSAGE_LABEL"), tr("Error message"));
	htmlTemplate.setContextParameter(QStringLiteral("ERROR_MESSAGE"), pStatus.toErrorDescription(true));
	//: ERROR ALL_PLATFORMS Invalid request by the browser, part of an HTML error page
	htmlTemplate.setContextParameter(QStringLiteral("REPORT_HEADER"), tr("Would you like to report this error?"));
	//: ERROR ALL_PLATFORMS Invalid request by the browser, part of an HTML error page
	htmlTemplate.setContextParameter(QStringLiteral("REPORT_LINK"), tr("https://www.ausweisapp.bund.de/en/qa/report-an-error/"));
	//: ERROR ALL_PLATFORMS Invalid request by the browser, part of an HTML error page
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
		const auto& url = QStringLiteral("<a href='%1'>%2</a>").arg(redirectAddressWithResult.toString(), redirectAddressWithResult.host());
		//: ERROR ALL_PLATFORMS The connection to the browser was lost/timed out..
		mSendError = tr("The connection to the browser was lost. No forwarding was executed. Please try to call the URL again manually: %1").arg(url);
		return false;
	}

	qCDebug(activation) << "Redirecting now to URL:" << redirectAddressWithResult;
	HttpResponse response;
	setCommonHeaders(response);
	response.setHeader(QByteArrayLiteral("Location"), redirectAddressWithResult.toEncoded());
	response.setStatus(HTTP_STATUS_SEE_OTHER);
	mRequest->send(response);
	return true;
}

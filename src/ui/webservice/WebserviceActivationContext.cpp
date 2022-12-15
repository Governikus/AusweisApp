/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "WebserviceActivationContext.h"

#include "LanguageLoader.h"
#include "NetworkManager.h"
#include "Template.h"
#include "UrlUtil.h"

#include <QFile>
#include <QLoggingCategory>
#include <QUrlQuery>


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(webservice)

void WebserviceActivationContext::setCommonHeaders(HttpResponse& pResponse) const
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
		setSendError(tr("The browser connection was lost."));
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
		setSendError(tr("The browser connection was lost."));
		return false;
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
	htmlTemplate.setContextParameter(QStringLiteral("CONTENT_LINK"), mRequest->getUrl().toString());
	//: ERROR ALL_PLATFORMS A new authentication request was received while the previous one was still running. Part of an HTML error page.
	htmlTemplate.setContextParameter(QStringLiteral("CONTENT_BUTTON"), tr("Try again"));
	QByteArray htmlPage = htmlTemplate.render().toUtf8();

	HttpResponse response(HTTP_STATUS_CONFLICT);
	response.setBody(htmlPage, QByteArrayLiteral("text/html; charset=utf-8"));
	mRequest->send(response);
	return true;
}


bool WebserviceActivationContext::sendErrorPage(http_status pStatusCode, const GlobalStatus& pStatus)
{
	if (!mRequest->isConnected())
	{
		//: ERROR ALL_PLATFORMS No HTTP connection present.
		setSendError(tr("The browser connection was lost."));
		return false;
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
	mRequest->send(response);
	return true;
}


bool WebserviceActivationContext::sendRedirect(const QUrl& pRedirectAddress, const GlobalStatus& pStatus)
{
	QUrl redirectAddressWithResult = UrlUtil::addMajorMinor(pRedirectAddress, pStatus);
	qCDebug(webservice) << "Redirect URL:" << redirectAddressWithResult;

	if (!mRequest->isConnected())
	{
		const auto& url = QStringLiteral("<a href='%1'>%2</a>").arg(redirectAddressWithResult.toString(), redirectAddressWithResult.host());
		//: ERROR ALL_PLATFORMS The connection to the browser was lost/timed out..
		setSendError(tr("The connection to the browser was lost. No forwarding was executed. Please try to call the URL again manually: %1").arg(url));
		return false;
	}

	qCDebug(webservice) << "Redirecting now to URL:" << redirectAddressWithResult;
	HttpResponse response(HTTP_STATUS_SEE_OTHER);
	setCommonHeaders(response);
	response.setHeader(QByteArrayLiteral("Location"), redirectAddressWithResult.toEncoded());
	mRequest->send(response);
	return true;
}

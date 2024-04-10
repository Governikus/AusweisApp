/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "HttpHandler.h"

#include "LanguageLoader.h"
#include "NetworkManager.h"
#include "Template.h"
#include "UrlUtil.h"
#include "VersionInfo.h"

#include <QFile>
#include <QLoggingCategory>
#include <QUrlQuery>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(network)


void HttpHandler::handle(const QSharedPointer<HttpRequest>& pRequest)
{
	const auto& url = pRequest->getUrl();
	if (url.path() == QLatin1String("/eID-Client"))
	{
		switch (pRequest->getHttpMethod())
		{
			case HTTP_GET:
				if (handleGetRequest(pRequest, url))
				{
					return;
				}
				break;

			case HTTP_OPTIONS:
			case HTTP_HEAD:
				qCDebug(network) << "Request type: Pre-flight CORS";
				handleCorsRequest(pRequest);
				return;

			default:
				qCDebug(network) << "Unhandled HTTP method:" << pRequest->getMethod();
		}
	}
	else if (url.path() == QLatin1String("/favicon.ico"))
	{
		handleImageRequest(pRequest, QStringLiteral(":/images/desktop/npa.ico")); // it MUST be an ICO!
		return;
	}
	else if (url.path().startsWith(QLatin1String("/images/")) && !url.path().contains(QLatin1String("../")))
	{
		handleImageRequest(pRequest, QStringLiteral(":%1").arg(url.path()));
		return;
	}

	qCWarning(network) << "Request type: unknown";
	const auto& statusMsg = NetworkManager::getFormattedStatusMessage(HTTP_STATUS_NOT_FOUND);

	Template htmlTemplate = Template::fromFile(QStringLiteral(":/template.html"));
	//: ERROR ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.
	htmlTemplate.setContextParameter(QStringLiteral("TITLE"), tr("Invalid request (%1)").arg(statusMsg));
	//: ERROR ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER"), tr("Invalid request (%1)").arg(statusMsg));
	//: ERROR ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER_EXPLANATION"), tr("Your browser sent a request that couldn't be interpreted."));
	//: ERROR ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_SUBHEADER_LABEL"), tr("Error message:"));
	//: ERROR ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_SUBHEADER"), tr("Unknown request: %1").arg(url.toString()));
	//: ERROR ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.
	htmlTemplate.setContextParameter(QStringLiteral("CONTENT_HEADER"), tr("Would you like to report this error?"));
	htmlTemplate.setContextParameter(QStringLiteral("CONTENT_LINK"), QStringLiteral("https://www.ausweisapp.bund.de/%1/aa2/report").arg(LanguageLoader::getLocaleCode()));
	//: ERROR ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.
	htmlTemplate.setContextParameter(QStringLiteral("CONTENT_BUTTON"), tr("Report now"));
	QByteArray htmlPage = htmlTemplate.render().toUtf8();

	HttpResponse response(HTTP_STATUS_NOT_FOUND);
	response.setBody(htmlPage, QByteArrayLiteral("text/html; charset=utf-8"));
	pRequest->send(response);
}


void HttpHandler::handleCorsRequest(const QSharedPointer<HttpRequest>& pRequest) const
{
	HttpResponse response(HTTP_STATUS_NO_CONTENT);
	setCorsResponseHeaders(response);
	pRequest->send(response);
}


bool HttpHandler::handleGetRequest(const QSharedPointer<HttpRequest>& pRequest, const QUrl& pUrl)
{
	const auto queryUrl = QUrlQuery(pUrl);
	UrlUtil::setHiddenSettings(queryUrl);
	const auto [type, value] = UrlUtil::getRequest(queryUrl);
	switch (type)
	{
		case UrlQueryRequest::SHOWUI:
		{
			qCDebug(network) << "Request type: showui";
			handleShowUiRequest(value, pRequest);
			return true;
		}

		case UrlQueryRequest::STATUS:
		{
			qCDebug(network) << "Request type: status";
			const StatusFormat statusFormat = UrlUtil::prepareToEnum(value, StatusFormat::PLAIN);
			handleStatusRequest(statusFormat, pRequest);
			return true;
		}

		case UrlQueryRequest::TCTOKENURL:
		{
			qCDebug(network) << "Request type: authentication";
			handleWorkflowRequest(pRequest);
			return true;
		}

		default:
			qCWarning(network) << "Unknown request type:" << pUrl;
			return false;
	}
}


void HttpHandler::handleImageRequest(const QSharedPointer<HttpRequest>& pRequest, const QString& pImagePath) const
{
	HttpResponse response;

	QFile imageFile(pImagePath);
	if (imageFile.open(QIODevice::ReadOnly))
	{
		response.setStatus(HTTP_STATUS_OK);
		response.setBody(imageFile.readAll(), guessImageContentType(pImagePath));
		imageFile.close();
	}
	else
	{
		qCCritical(network) << "Unknown image file requested" << pImagePath;
		response.setStatus(HTTP_STATUS_NOT_FOUND);
	}

	pRequest->send(response);
}


QByteArray HttpHandler::guessImageContentType(const QString& pFileName) const
{
	if (pFileName.endsWith(QLatin1String(".ico"), Qt::CaseInsensitive))
	{
		return QByteArrayLiteral("image/x-icon");
	}
	if (pFileName.endsWith(QLatin1String(".jpg"), Qt::CaseInsensitive) || pFileName.endsWith(QLatin1String(".jpeg"), Qt::CaseInsensitive))
	{
		return QByteArrayLiteral("image/jpeg");
	}
	if (pFileName.endsWith(QLatin1String(".png"), Qt::CaseInsensitive))
	{
		return QByteArrayLiteral("image/png");
	}
	if (pFileName.endsWith(QLatin1String(".svg"), Qt::CaseInsensitive))
	{
		return QByteArrayLiteral("image/svg+xml");
	}
	qCWarning(network) << "Unknown content type, returning default for image" << pFileName;
	return QByteArrayLiteral("image");
}


void HttpHandler::setCorsResponseHeaders(HttpResponse& pResponse) const
{
	pResponse.setHeader(QByteArrayLiteral("Access-Control-Allow-Origin"), QByteArrayLiteral("*"));
	pResponse.setHeader(QByteArrayLiteral("Access-Control-Allow-Private-Network"), QByteArrayLiteral("true"));
}


void HttpHandler::handleStatusRequest(StatusFormat pStatusFormat, const QSharedPointer<HttpRequest>& pRequest) const
{
	qCDebug(network) << "Create response with status format:" << pStatusFormat;

	HttpResponse response(HTTP_STATUS_OK);
	setCorsResponseHeaders(response);
	switch (pStatusFormat)
	{
		case StatusFormat::PLAIN:
			response.setBody(VersionInfo::getInstance().toText().toUtf8(), QByteArrayLiteral("text/plain; charset=utf-8"));
			break;

		case StatusFormat::JSON:
			response.setBody(VersionInfo::getInstance().toJson(), QByteArrayLiteral("application/json"));
			break;
	}

	pRequest->send(response);
}


#include "moc_HttpHandler.cpp"

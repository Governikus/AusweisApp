/*!
 * WebserviceActivationHandler.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "WebserviceActivationHandler.h"

#include "HttpServerStatusParser.h"
#include "Template.h"
#include "VersionInfo.h"
#include "VersionNumber.h"
#include "WebserviceActivationContext.h"

#include <QCoreApplication>
#include <QFile>
#include <QLoggingCategory>
#include <QUrlQuery>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(activation)

quint16 WebserviceActivationHandler::PORT = 24727;


WebserviceActivationHandler::WebserviceActivationHandler()
	: ActivationHandler()
	, mHttpServer()
{
}


WebserviceActivationHandler::~WebserviceActivationHandler()
{
}


quint16 WebserviceActivationHandler::getServerPort()
{
	return mHttpServer.serverPort();
}


void WebserviceActivationHandler::stop()
{
	mHttpServer.close();
}


bool WebserviceActivationHandler::start()
{
	if (mHttpServer.listen(QHostAddress::LocalHost, PORT))
	{
		connect(&mHttpServer, &QHttpServer::newRequest, this, &WebserviceActivationHandler::onNewRequest);
		qCDebug(activation) << "Listening on port:" << mHttpServer.serverPort();
		return true;
	}

	HttpServerStatusParser parser(PORT);
	QString serverAppName = parser.request() ? parser.getVersionInfo().getName() : parser.getServerHeader();
	if (serverAppName.startsWith(VersionInfo::getInstance().getName()))
	{
		qDebug() << "We are already started... calling ShowUI";
		HttpServerRequestor requestor;
		if (requestor.request(HttpServerRequestor::createUrl("ShowUI=" + UiModule::CURRENT, PORT)).isNull())
		{
			qCWarning(activation) << "ShowUI request timed out";
		}
	}
	else
	{
		qCCritical(activation) << "Cannot start application. Port on localhost is already bound by another program:" << serverAppName;

		QString msg = serverAppName.isEmpty() ? tr("An unknown program uses the required port (%1). Please exit the other program and try again!").arg(PORT) :
				tr("Another program (%1) uses the required port (%2). Please exit this other program and try again!").arg(serverAppName).arg(PORT);
		Q_EMIT fireShowUserInformation(msg);
	}
	return false;
}


void WebserviceActivationHandler::onNewRequest(QSharedPointer<QHttpRequest> pRequest, QSharedPointer<QHttpResponse> pResponse)
{
	qCDebug(activation) << "Got new request";
	qCDebug(activation) << "Request URL:" << pRequest->url();
	qCDebug(activation) << "Request User-Agent:" << pRequest->header(QStringLiteral("User-Agent"));

	if (pRequest->url().path() == QLatin1String("/eID-Client"))
	{
		const auto urlParameter = getQueryParameter(pRequest->url());

		if (urlParameter.contains(QLatin1String("showui")))
		{
			qCDebug(activation) << "Request type: showui";
			UiModule module = Enum<UiModule>::fromString(urlParameter.value(QLatin1String("showui")).toUpper(), UiModule::DEFAULT);
			handleShowUiRequest(module, pRequest, pResponse);
			return;
		}
		else if (urlParameter.contains(QLatin1String("status")))
		{
			qCDebug(activation) << "Request type: status";
			StatusFormat statusFormat = Enum<StatusFormat>::fromString(urlParameter.value(QLatin1String("status")).toUpper(), StatusFormat::PLAIN);
			handleStatusRequest(statusFormat, pResponse);
			return;
		}
		else if (urlParameter.contains(QLatin1String("tctokenurl")))
		{
			qDebug(activation) << "Request type: authentication";
			Q_EMIT fireAuthenticationRequest(new WebserviceActivationContext(pRequest, pResponse));
			return;
		}
	}
	else if (pRequest->url().path() == QLatin1String("/favicon.ico"))
	{
		qCDebug(activation) << "Request image:" << pRequest->url().path();
		handleImageRequest(pResponse, QStringLiteral(":/images/npa.ico")); // it MUST be an ICO!
		return;
	}
	else if (pRequest->url().path().startsWith(QLatin1String("/images/")) && !pRequest->url().path().contains(QLatin1String("../")))
	{
		qCDebug(activation) << "Request image:" << pRequest->url().path();
		handleImageRequest(pResponse, QStringLiteral(":%1").arg(pRequest->url().path()));
		return;
	}

	qCWarning(activation) << "Request type: unknown";

	Template htmlTemplate = Template::fromFile(QStringLiteral(":/html_templates/error.html"));
	htmlTemplate.setContextParameter(QStringLiteral("TITLE"), tr("404 Not found"));
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER"), tr("Invalid request"));
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER_EXPLANATION"), tr("Your browser sent a request that couldn't be interpreted."));
	htmlTemplate.setContextParameter(QStringLiteral("ERROR_MESSAGE_LABEL"), tr("Error message"));
	htmlTemplate.setContextParameter(QStringLiteral("ERROR_MESSAGE"), tr("Unknown request: %1").arg(pRequest->url().toString()));
	htmlTemplate.setContextParameter(QStringLiteral("REPORT_HEADER"), tr("Would you like to report this error?"));
	htmlTemplate.setContextParameter(QStringLiteral("REPORT_LINK"), tr("https://www.ausweisapp.bund.de/en/feedback/melden-sie-einen-fehler/"));
	htmlTemplate.setContextParameter(QStringLiteral("REPORT_BUTTON"), tr("Report now"));
	QByteArray htmlPage = htmlTemplate.render().toUtf8();

	WebserviceActivationContext::setServerHeader(*pResponse.data());
	pResponse->setHeader(QStringLiteral("Content-Type"), QStringLiteral("text/html; charset=utf-8"));
	pResponse->setHeader(QStringLiteral("Content-Length"), QString::number(htmlPage.size()));
	pResponse->writeHead(QHttpResponse::STATUS_NOT_FOUND);
	pResponse->end(htmlPage);
}


void WebserviceActivationHandler::handleShowUiRequest(UiModule pUiModule, QSharedPointer<QHttpRequest> pRequest, QSharedPointer<QHttpResponse> pResponse)
{
	WebserviceActivationContext::setServerHeader(*pResponse.data());
	pResponse->setHeader(QStringLiteral("Content-Type"), QStringLiteral("text/plain; charset=utf-8"));
	pResponse->setHeader(QStringLiteral("Content-Length"), QString::number(0));
	pResponse->writeHead(QHttpResponse::STATUS_OK);
	pResponse->end();

	QString userAgent = pRequest->header(QStringLiteral("User-Agent"));
	if (userAgent.startsWith(QCoreApplication::applicationName()))
	{
		QString version = userAgent.remove(QCoreApplication::applicationName() + '/').split(' ').at(0);
		VersionNumber callerVersion(version);

		if (callerVersion > VersionNumber::getApplicationVersion())
		{
			qCWarning(activation) << "Current version is lower than caller version";
			Q_EMIT fireShowUserInformation(tr("You tried to start a newer version (%1) of currently running application. Please stop the current version (%2) and start again!").arg(version, QCoreApplication::applicationVersion()));
			return;
		}
		else if (callerVersion < VersionNumber::getApplicationVersion())
		{
			qCWarning(activation) << "Current version is higher than caller version";
			Q_EMIT fireShowUserInformation(tr("You tried to start an older version (%1) of currently running application. Please open the currently running version (%2)!").arg(version, QCoreApplication::applicationVersion()));
			return;
		}
	}

	Q_EMIT fireShowUiRequest(pUiModule);
}


void WebserviceActivationHandler::handleImageRequest(QSharedPointer<QHttpResponse> pResponse, const QString& pImagePath)
{
	QByteArray content;
	QString contentType;
	QHttpResponse::StatusCode statusCode;

	QFile imageFile(pImagePath);
	if (imageFile.open(QIODevice::ReadOnly))
	{
		content = imageFile.readAll();
		imageFile.close();
		contentType = guessImageContentType(pImagePath);
		statusCode = QHttpResponse::STATUS_OK;
	}
	else
	{
		qCCritical(activation) << "Unknown image file requested" << pImagePath;
		content = QByteArrayLiteral("Not found");
		contentType = QStringLiteral("text/plain; charset=utf-8");
		statusCode = QHttpResponse::STATUS_NOT_FOUND;
	}

	WebserviceActivationContext::setServerHeader(*pResponse);
	pResponse->setHeader(QStringLiteral("Content-Type"), contentType);
	pResponse->setHeader(QStringLiteral("Content-Length"), QString::number(content.size()));
	pResponse->writeHead(statusCode);
	pResponse->end(content);
}


QString WebserviceActivationHandler::guessImageContentType(const QString& pFileName) const
{
	if (pFileName.endsWith(QLatin1String(".ico"), Qt::CaseInsensitive))
	{
		return QStringLiteral("image/x-icon");
	}
	if (pFileName.endsWith(QLatin1String(".jpg"), Qt::CaseInsensitive) || pFileName.endsWith(QLatin1String(".jpeg"), Qt::CaseInsensitive))
	{
		return QStringLiteral("image/jpeg");
	}
	if (pFileName.endsWith(QLatin1String(".png"), Qt::CaseInsensitive))
	{
		return QStringLiteral("image/png");
	}
	if (pFileName.endsWith(QLatin1String(".svg"), Qt::CaseInsensitive))
	{
		return QStringLiteral("image/svg+xml");
	}
	qWarning() << "Unknown content type, returing default for image" << pFileName;
	return QStringLiteral("image");
}


void WebserviceActivationHandler::handleStatusRequest(StatusFormat pStatusFormat, QSharedPointer<QHttpResponse> pResponse)
{
	qCDebug(activation) << "Create response with status format:" << pStatusFormat;
	QByteArray versionInfo;
	switch (pStatusFormat)
	{
		case StatusFormat::PLAIN:
			pResponse->setHeader(QStringLiteral("Content-Type"), QStringLiteral("text/plain; charset=utf-8"));
			versionInfo = VersionInfo::getInstance().toText().toUtf8();
			break;

		case StatusFormat::JSON:
			pResponse->setHeader(QStringLiteral("Content-Type"), QStringLiteral("application/json"));
			versionInfo = VersionInfo::getInstance().toJson();
			break;
	}

	WebserviceActivationContext::setServerHeader(*pResponse.data());
	pResponse->setHeader(QStringLiteral("Access-Control-Allow-Origin"), QStringLiteral("*"));
	pResponse->setHeader(QStringLiteral("Content-Length"), QString::number(versionInfo.size()));
	pResponse->writeHead(QHttpResponse::STATUS_OK);
	pResponse->end(versionInfo);
}

/*!
 * WebserviceActivationHandler.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "WebserviceActivationHandler.h"

#include "EnvHolder.h"
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

WebserviceActivationHandler::WebserviceActivationHandler()
	: ActivationHandler()
	, mServer()
{
}


WebserviceActivationHandler::~WebserviceActivationHandler()
{
}


void WebserviceActivationHandler::stop()
{
	mServer.reset();
}


bool WebserviceActivationHandler::start()
{
	mServer = EnvHolder::shared<HttpServer>();

	if (mServer->isListening())
	{
		connect(mServer.data(), &HttpServer::fireNewHttpRequest, this, &WebserviceActivationHandler::onNewRequest);
		return true;
	}


	const int port = HttpServer::cPort;
	HttpServerStatusParser parser(port);
	QString serverAppName = parser.request() ? parser.getVersionInfo().getName() : parser.getServerHeader();
	if (serverAppName.startsWith(VersionInfo::getInstance().getName()))
	{
		qCDebug(activation) << "We are already started... calling ShowUI";
		HttpServerRequestor requestor;
		if (requestor.request(HttpServerRequestor::createUrl("ShowUI=" + UiModule::CURRENT, port)).isNull())
		{
			qCWarning(activation) << "ShowUI request timed out";
		}
	}
	else
	{
		qCCritical(activation) << "Cannot start application. Port on localhost is already bound by another program:" << serverAppName;

		QString msg = serverAppName.isEmpty() ? tr("An unknown program uses the required port (%1). Please exit the other program and try again!").arg(port) :
				tr("Another program (%1) uses the required port (%2). Please exit this other program and try again!").arg(serverAppName).arg(port);
		Q_EMIT fireShowUserInformation(msg);
	}

	mServer.reset();
	return false;
}


void WebserviceActivationHandler::onNewRequest(const QSharedPointer<HttpRequest>& pRequest)
{
	const auto& url = pRequest->getUrl();
	if (url.path() == QLatin1String("/eID-Client"))
	{
		const auto urlParameter = getQueryParameter(url);

		if (urlParameter.contains(QLatin1String("showui")))
		{
			qCDebug(activation) << "Request type: showui";
			UiModule module = Enum<UiModule>::fromString(urlParameter.value(QLatin1String("showui")).toUpper(), UiModule::DEFAULT);
			handleShowUiRequest(module, pRequest);
			return;
		}
		else if (urlParameter.contains(QLatin1String("status")))
		{
			qCDebug(activation) << "Request type: status";
			StatusFormat statusFormat = Enum<StatusFormat>::fromString(urlParameter.value(QLatin1String("status")).toUpper(), StatusFormat::PLAIN);
			handleStatusRequest(statusFormat, pRequest);
			return;
		}
		else if (urlParameter.contains(QLatin1String("tctokenurl")))
		{
			qCDebug(activation) << "Request type: authentication";
			Q_EMIT fireAuthenticationRequest(new WebserviceActivationContext(pRequest));
			return;
		}
	}
	else if (url.path() == QLatin1String("/favicon.ico"))
	{
		handleImageRequest(pRequest, QStringLiteral(":/images/npa.ico")); // it MUST be an ICO!
		return;
	}
	else if (url.path().startsWith(QLatin1String("/images/")) && !url.path().contains(QLatin1String("../")))
	{
		handleImageRequest(pRequest, QStringLiteral(":%1").arg(url.path()));
		return;
	}

	qCWarning(activation) << "Request type: unknown";

	Template htmlTemplate = Template::fromFile(QStringLiteral(":/html_templates/error.html"));
	htmlTemplate.setContextParameter(QStringLiteral("TITLE"), tr("404 Not found"));
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER"), tr("Invalid request"));
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER_EXPLANATION"), tr("Your browser sent a request that couldn't be interpreted."));
	htmlTemplate.setContextParameter(QStringLiteral("ERROR_MESSAGE_LABEL"), tr("Error message"));
	htmlTemplate.setContextParameter(QStringLiteral("ERROR_MESSAGE"), tr("Unknown request: %1").arg(url.toString()));
	htmlTemplate.setContextParameter(QStringLiteral("REPORT_HEADER"), tr("Would you like to report this error?"));
	htmlTemplate.setContextParameter(QStringLiteral("REPORT_LINK"), tr("https://www.ausweisapp.bund.de/en/feedback/melden-sie-einen-fehler/"));
	htmlTemplate.setContextParameter(QStringLiteral("REPORT_BUTTON"), tr("Report now"));
	QByteArray htmlPage = htmlTemplate.render().toUtf8();

	HttpResponse response;
	response.setStatus(HttpStatusCode::NOT_FOUND);
	response.setBody(htmlPage, QByteArrayLiteral("text/html; charset=utf-8"));
	pRequest->send(response);
}


void WebserviceActivationHandler::handleShowUiRequest(UiModule pUiModule, const QSharedPointer<HttpRequest>& pRequest)
{
	pRequest->send(HttpStatusCode::OK);

	QString userAgent = QString::fromLatin1(pRequest->getHeader(QByteArrayLiteral("user-agent")));
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


void WebserviceActivationHandler::handleImageRequest(const QSharedPointer<HttpRequest>& pRequest, const QString& pImagePath)
{
	HttpResponse response;

	QFile imageFile(pImagePath);
	if (imageFile.open(QIODevice::ReadOnly))
	{
		response.setStatus(HttpStatusCode::OK);
		response.setBody(imageFile.readAll(), guessImageContentType(pImagePath));
		imageFile.close();
	}
	else
	{
		qCCritical(activation) << "Unknown image file requested" << pImagePath;
		response.setStatus(HttpStatusCode::NOT_FOUND);
		response.setBody(QByteArrayLiteral("Not found"), QByteArrayLiteral("text/plain; charset=utf-8"));
	}

	pRequest->send(response);
}


QByteArray WebserviceActivationHandler::guessImageContentType(const QString& pFileName) const
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
	qCWarning(activation) << "Unknown content type, returing default for image" << pFileName;
	return QByteArrayLiteral("image");
}


void WebserviceActivationHandler::handleStatusRequest(StatusFormat pStatusFormat, const QSharedPointer<HttpRequest>& pRequest)
{
	qCDebug(activation) << "Create response with status format:" << pStatusFormat;

	HttpResponse response(HttpStatusCode::OK);
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

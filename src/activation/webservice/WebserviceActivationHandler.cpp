/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "WebserviceActivationHandler.h"

#include "Env.h"
#include "HttpServerStatusParser.h"
#include "LanguageLoader.h"
#include "NetworkManager.h"
#include "Template.h"
#include "VersionInfo.h"
#include "VersionNumber.h"
#include "WebserviceActivationContext.h"

#include <QCoreApplication>
#include <QFile>
#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(activation)


WebserviceActivationHandler::WebserviceActivationHandler()
	: ActivationHandler()
	, mServer()
{
}


void WebserviceActivationHandler::stop()
{
	mServer.reset();
}


bool WebserviceActivationHandler::start()
{
	mServer = Env::getShared<HttpServer>();

	if (mServer->isListening())
	{
		connect(mServer.data(), &HttpServer::fireNewHttpRequest, this, &WebserviceActivationHandler::onNewRequest);
		return true;
	}

	QString showMessage;
	const quint16 port = HttpServer::cPort;
	for (const auto& address : qAsConst(HttpServer::cAddresses))
	{
		HttpServerStatusParser parser(port, address);
		QString serverAppName = parser.request() ? parser.getVersionInfo().getName() : parser.getServerHeader();
		if (serverAppName.startsWith(VersionInfo::getInstance().getName()))
		{
			qCDebug(activation) << "We are already started... calling ShowUI";
			HttpServerRequestor requestor;
			if (requestor.getRequest(HttpServerRequestor::createUrl(QStringLiteral("ShowUI=") + UiModule::CURRENT, port, address)).isNull())
			{
				qCWarning(activation) << "ShowUI request timed out";
			}
			showMessage.clear();
			break;
		}
		else
		{
			qCCritical(activation) << "Cannot start application. Port on address is probably already bound by another program:" << serverAppName;

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
		Q_EMIT fireShowUserInformation(showMessage);
	}

	mServer.reset();
	return false;
}


void WebserviceActivationHandler::onNewRequest(const QSharedPointer<HttpRequest>& pRequest)
{
	const auto& url = pRequest->getUrl();
	if (url.path() == QLatin1String("/eID-Client"))
	{
		const auto queryUrl = QUrlQuery(url);
		handleQueryParams(queryUrl);
		const auto [type, value] = getRequest(queryUrl);
		switch (type)
		{
			case ActivationType::SHOWUI:
			{
				qCDebug(activation) << "Request type: showui";
				UiModule showModule = Enum<UiModule>::fromString(value.toUpper(), UiModule::DEFAULT);
				handleShowUiRequest(showModule, pRequest);
				return;
			}

			case ActivationType::STATUS:
			{
				qCDebug(activation) << "Request type: status";
				StatusFormat statusFormat = Enum<StatusFormat>::fromString(value.toUpper(), StatusFormat::PLAIN);
				handleStatusRequest(statusFormat, pRequest);
				return;
			}

			case ActivationType::TCTOKENURL:
				qCDebug(activation) << "Request type: authentication";
				Q_EMIT fireAuthenticationRequest(QSharedPointer<WebserviceActivationContext>::create(pRequest));
				return;

			default:
				qCWarning(activation) << "Unknown request type:" << url;
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

	qCWarning(activation) << "Request type: unknown";
	const auto& statusMsg = NetworkManager::getFormattedStatusMessage(HTTP_STATUS_NOT_FOUND);

	Template htmlTemplate = Template::fromFile(QStringLiteral(":/html_templates/error.html"));
	//: ERROR ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.
	htmlTemplate.setContextParameter(QStringLiteral("TITLE"), tr("Invalid request (%1)").arg(statusMsg));
	//: ERROR ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER"), tr("Invalid request (%1)").arg(statusMsg));
	//: ERROR ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.
	htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER_EXPLANATION"), tr("Your browser sent a request that couldn't be interpreted."));
	//: ERROR ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.
	htmlTemplate.setContextParameter(QStringLiteral("ERROR_MESSAGE_LABEL"), tr("Error message"));
	//: ERROR ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.
	htmlTemplate.setContextParameter(QStringLiteral("ERROR_MESSAGE"), tr("Unknown request: %1").arg(url.toString()));
	//: ERROR ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.
	htmlTemplate.setContextParameter(QStringLiteral("REPORT_HEADER"), tr("Would you like to report this error?"));
	htmlTemplate.setContextParameter(QStringLiteral("REPORT_LINK"), QStringLiteral("https://www.ausweisapp.bund.de/%1/aa2/report").arg(LanguageLoader::getLocaleCode()));
	//: ERROR ALL_PLATFORMS The browser sent an unknown or faulty request, part of an HTML error page.
	htmlTemplate.setContextParameter(QStringLiteral("REPORT_BUTTON"), tr("Report now"));
	QByteArray htmlPage = htmlTemplate.render().toUtf8();

	HttpResponse response;
	response.setStatus(HTTP_STATUS_NOT_FOUND);
	response.setBody(htmlPage, QByteArrayLiteral("text/html; charset=utf-8"));
	pRequest->send(response);
}


void WebserviceActivationHandler::handleShowUiRequest(UiModule pUiModule, const QSharedPointer<HttpRequest>& pRequest)
{
	pRequest->send(HTTP_STATUS_OK);

	QString userAgent = QString::fromLatin1(pRequest->getHeader(QByteArrayLiteral("user-agent")));
	if (userAgent.startsWith(QCoreApplication::applicationName()))
	{
		QString version = userAgent.remove(QCoreApplication::applicationName() + QLatin1Char('/')).split(QLatin1Char(' ')).at(0);
		VersionNumber callerVersion(version);

		if (callerVersion > VersionNumber::getApplicationVersion())
		{
			qCWarning(activation) << "Current version is lower than caller version";
			//: ERROR ALL_PLATFORMS The external request to show the UI requested a newer version than the one currently installed.
			Q_EMIT fireShowUserInformation(tr("You tried to start a newer version (%1) of currently running AusweisApp2. Please stop the current version (%2) and start again!").arg(version, QCoreApplication::applicationVersion()));
			return;
		}
		else if (callerVersion < VersionNumber::getApplicationVersion())
		{
			qCWarning(activation) << "Current version is higher than caller version";
			//: ERROR ALL_PLATFORMS The external request to show the UI requested an older version than the one currently installed.
			Q_EMIT fireShowUserInformation(tr("You tried to start an older version (%1) of currently running AusweisApp2. Please open the currently running version (%2)!").arg(version, QCoreApplication::applicationVersion()));
			return;
		}
	}

	Q_EMIT fireShowUiRequest(pUiModule);
}


void WebserviceActivationHandler::handleImageRequest(const QSharedPointer<HttpRequest>& pRequest, const QString& pImagePath) const
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
		qCCritical(activation) << "Unknown image file requested" << pImagePath;
		response.setStatus(HTTP_STATUS_NOT_FOUND);
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
	qCWarning(activation) << "Unknown content type, returning default for image" << pFileName;
	return QByteArrayLiteral("image");
}


void WebserviceActivationHandler::handleStatusRequest(StatusFormat pStatusFormat, const QSharedPointer<HttpRequest>& pRequest) const
{
	qCDebug(activation) << "Create response with status format:" << pStatusFormat;

	HttpResponse response(HTTP_STATUS_OK);
	response.setHeader(QByteArrayLiteral("Access-Control-Allow-Origin"), QByteArrayLiteral("*"));
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

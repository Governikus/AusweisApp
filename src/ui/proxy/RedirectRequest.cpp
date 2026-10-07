/**
 * Copyright (c) 2022-2026 Governikus Service GmbH, Germany
 */

#include "RedirectRequest.h"

#include "LanguageLoader.h"
#include "Template.h"

#include <QCoreApplication>
#include <QHostAddress>
#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(rproxy)

RedirectRequest::RedirectRequest(const QSharedPointer<HttpRequest>& pRequest, QObject* pParent)
	: QObject(pParent)
	, mSocket()
	, mRequest(pRequest)
	, mPortWrapper(pRequest->getLocalPort(), pRequest->getPeerPort())
	, mAnswerReceived(false)
{
	Q_ASSERT(mRequest);

	connect(mRequest.data(), &HttpRequest::fireSocketStateChanged, this, [this](QAbstractSocket::SocketState pSocketState)
			{
				if (pSocketState == QAbstractSocket::UnconnectedState)
				{
					deleteLater();
				}
			});

	connect(&mSocket, &QAbstractSocket::disconnected, this, &QObject::deleteLater);

	connect(&mSocket, &QAbstractSocket::errorOccurred, this, [this] {
				if (!isAnswerReceived())
				{
					qCWarning(rproxy) << "Cannot redirect:" << mSocket.error();
					redirect();
					return;
				}
				deleteLater();
			});

	connect(&mSocket, &QAbstractSocket::readyRead, this, [this] {
				mRequest->send(mSocket.readAll());
				answerReceived();
			});

	connect(&mSocket, &QAbstractSocket::connected, this, [this] {
				if (qEnvironmentVariableIsSet("AUSWEISAPP_PROXY_USE_REDIRECT"))
				{
					sendHttpRedirect();
					answerReceived();
					deleteLater();
				}
				else
				{
					mRequest->triggerSocketBuffer();
				}
			});

	connect(mRequest.data(), &HttpRequest::fireSocketBuffer, this, [this] (const QByteArray& pBuffer){
				mSocket.write(pBuffer);
				mSocket.flush();
			});

	if (mPortWrapper.isEmpty())
	{
		qCWarning(rproxy) << "No port found";
		deleteLater();
	}
	else
	{
		redirect();
	}
}


RedirectRequest::~RedirectRequest()
{
	if (!mAnswerReceived)
	{
		Template htmlTemplate = Template::fromFile(QStringLiteral(":/template.html"));
		//: ALL_PLATFORMS The local AusweisApp (access via reverse proxy) is not reachable, part of an HTML error page.
		htmlTemplate.setContextParameter(QStringLiteral("TITLE"), tr("Cannot reach local %1").arg(QCoreApplication::applicationName()));
		htmlTemplate.setContextParameter(QStringLiteral("APPLICATION_LINK"), QStringLiteral("https://www.ausweisapp.bund.de/%1").arg(LanguageLoader::getLocaleCode()));
		//: ALL_PLATFORMS The local AusweisApp (access via reverse proxy) is not reachable, part of an HTML error page.
		htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER"), tr("Cannot reach local %1").arg(QCoreApplication::applicationName()));
		//: ALL_PLATFORMS The local AusweisApp (access via reverse proxy) is not reachable, part of an HTML error page.
		htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER_EXPLANATION"), tr("Your local %1 is not running. Please start your local %1 and try again.").arg(QCoreApplication::applicationName()));
		//: ALL_PLATFORMS The local AusweisApp (access via reverse proxy) is not reachable, part of an HTML error page.
		htmlTemplate.setContextParameter(QStringLiteral("CONTENT_HEADER"), tr("Would you like to try again?"));
		htmlTemplate.setContextParameter(QStringLiteral("CONTENT_LINK"), mRequest->getUrl().toString());
		//: ALL_PLATFORMS The local AusweisApp (access via reverse proxy) is not reachable, part of an HTML error page.
		htmlTemplate.setContextParameter(QStringLiteral("CONTENT_BUTTON"), tr("Try again"));
		htmlTemplate.setContextParameter(QStringLiteral("AUSWEISAPP_LOGO"), QStringLiteral("/images/html_templates/ausweisapp_logo_%1.svg").arg(LanguageLoader::getLocaleCode()));
		QByteArray htmlPage = htmlTemplate.render().toUtf8();

		HttpResponse response(HTTP_STATUS_BAD_GATEWAY);
		response.setBody(htmlPage, QByteArrayLiteral("text/html; charset=utf-8"));
		mRequest->send(response);
	}
}


void RedirectRequest::sendHttpRedirect()
{
	const auto& scheme = mRequest->isUpgrade() ? QByteArrayLiteral("ws://") : QByteArrayLiteral("http://");
	const auto host = mRequest->getHeader(QByteArrayLiteral("host")).replace(QByteArray::number(mRequest->getLocalPort()), QByteArray::number(mSocket.peerPort()));
	const auto url = scheme + host + mRequest->getUrl().toString().toLatin1();

	HttpResponse response(HTTP_STATUS_TEMPORARY_REDIRECT);
	response.setHeader(QByteArrayLiteral("Location"), url);
	mRequest->send(response);
}


void RedirectRequest::redirect()
{
	const auto port = mPortWrapper.pop();
	if (port > 0)
	{
		qCDebug(rproxy) << "Redirect to port:" << port;
		mSocket.connectToHost(QHostAddress::LocalHost, port);
	}
	else
	{
		qCDebug(rproxy) << "No port left";
		deleteLater();
	}
}


void RedirectRequest::answerReceived()
{
	mAnswerReceived = true;
}


bool RedirectRequest::isAnswerReceived() const
{
	return mAnswerReceived;
}

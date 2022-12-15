/*!
 * \copyright Copyright (c) 2022 Governikus GmbH & Co. KG, Germany
 */

#include "RedirectRequest.h"

#include "LanguageLoader.h"
#include "Template.h"

#include <QHostAddress>
#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(rproxy)

RedirectRequest::RedirectRequest(const QSharedPointer<HttpRequest>& pRequest, QObject* pParent)
	: QTcpSocket(pParent)
	, mRequest(pRequest)
	, mPortWrapper(pRequest)
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

	connect(this, &QAbstractSocket::disconnected, this, &QObject::deleteLater);

	connect(this, &QAbstractSocket::errorOccurred, this, [this] {
			if (!isAnswerReceived())
			{
				qCWarning(rproxy) << "Cannot redirect:" << error();
				mPortWrapper.invalidate();
				redirect();
				return;
			}
			deleteLater();
		});

	connect(this, &QAbstractSocket::readyRead, this, [this] {
			mRequest->send(readAll());
			answerReceived();
		});

	connect(this, &QAbstractSocket::connected, this, [this] {
			if (qEnvironmentVariableIsSet("AUSWEISAPP2_PROXY_USE_REDIRECT"))
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
			write(pBuffer);
			flush();
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
		//: ERROR ALL_PLATFORMS The local AusweisApp (access via reverse proxy) is not reachable, part of an HTML error page.
		htmlTemplate.setContextParameter(QStringLiteral("TITLE"), tr("Cannot reach local AusweisApp2"));
		htmlTemplate.setContextParameter(QStringLiteral("APPLICATION_LINK"), QStringLiteral("https://www.ausweisapp.bund.de/%1").arg(LanguageLoader::getLocaleCode()));
		//: ERROR ALL_PLATFORMS The local AusweisApp (access via reverse proxy) is not reachable, part of an HTML error page.
		htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER"), tr("Cannot reach local AusweisApp2"));
		//: ERROR ALL_PLATFORMS The local AusweisApp (access via reverse proxy) is not reachable, part of an HTML error page.
		htmlTemplate.setContextParameter(QStringLiteral("MESSAGE_HEADER_EXPLANATION"), tr("Your local AusweisApp2 is not running. Please start your local AusweisApp2 and try again."));
		//: ERROR ALL_PLATFORMS The local AusweisApp (access via reverse proxy) is not reachable, part of an HTML error page.
		htmlTemplate.setContextParameter(QStringLiteral("CONTENT_HEADER"), tr("Would you like to try again?"));
		htmlTemplate.setContextParameter(QStringLiteral("CONTENT_LINK"), mRequest->getUrl().toString());
		//: ERROR ALL_PLATFORMS The local AusweisApp (access via reverse proxy) is not reachable, part of an HTML error page.
		htmlTemplate.setContextParameter(QStringLiteral("CONTENT_BUTTON"), tr("Try again"));
		QByteArray htmlPage = htmlTemplate.render().toUtf8();

		HttpResponse response(HTTP_STATUS_BAD_GATEWAY);
		response.setBody(htmlPage, QByteArrayLiteral("text/html; charset=utf-8"));
		mRequest->send(response);
	}
}


void RedirectRequest::sendHttpRedirect()
{
	const auto& scheme = mRequest->isUpgrade() ? QByteArrayLiteral("ws://") : QByteArrayLiteral("http://");
	const auto host = mRequest->getHeader(QByteArrayLiteral("host")).replace(QByteArray::number(mRequest->getLocalPort()), QByteArray::number(peerPort()));
	const auto url = scheme + host + mRequest->getUrl().toString().toLatin1();

	HttpResponse response(HTTP_STATUS_TEMPORARY_REDIRECT);
	response.setHeader(QByteArrayLiteral("Location"), url);
	mRequest->send(response);
}


void RedirectRequest::redirect()
{
	const auto port = mPortWrapper.fetchPort();
	if (port > 0)
	{
		qCDebug(rproxy) << "Redirect to port:" << port;
		connectToHost(QHostAddress::LocalHost, port);
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

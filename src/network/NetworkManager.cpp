/*
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "NetworkManager.h"

#include "AppSettings.h"
#include "NetworkReplyError.h"
#include "NetworkReplyTimeout.h"
#include "VersionInfo.h"

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QNetworkProxyFactory>
#include <QRegularExpression>
#include <QStringBuilder>


using namespace governikus;

Q_GLOBAL_STATIC(NetworkManager, Instance)

Q_DECLARE_LOGGING_CATEGORY(network)


QString NetworkManager::getTlsVersionString(QSsl::SslProtocol pProtocol)
{
	switch (pProtocol)
	{
		case QSsl::SslProtocol::AnyProtocol:
			return QStringLiteral("AnyProtocol");

		case QSsl::SslProtocol::SecureProtocols:
			return QStringLiteral("SecureProtocols");

		case QSsl::SslProtocol::SslV2:
			return QStringLiteral("SslV2");

		case QSsl::SslProtocol::SslV3:
			return QStringLiteral("SslV3");

		case QSsl::SslProtocol::TlsV1SslV3:
			return QStringLiteral("TlsV1SslV3");

		case QSsl::SslProtocol::TlsV1_0:
			return QStringLiteral("TlsV1_0");

		case QSsl::SslProtocol::TlsV1_0OrLater:
			return QStringLiteral("TlsV1_0OrLater");

		case QSsl::SslProtocol::TlsV1_1:
			return QStringLiteral("TlsV1_1");

		case QSsl::SslProtocol::TlsV1_1OrLater:
			return QStringLiteral("TlsV1_1OrLater");

		case QSsl::SslProtocol::TlsV1_2:
			return QStringLiteral("TlsV1_2");

		case QSsl::SslProtocol::TlsV1_2OrLater:
			return QStringLiteral("TlsV1_2OrLater");

		case QSsl::SslProtocol::UnknownProtocol:
			return QStringLiteral("UnknownProtocol");
	}

	Q_UNREACHABLE();
}


int NetworkManager::getOpenConnectionCount()
{
	return mOpenConnectionCount;
}


QDebug operator <<(QDebug pDbg, QSsl::SslProtocol pProtocol)
{
	QDebugStateSaver saver(pDbg);
	pDbg.nospace() << NetworkManager::getTlsVersionString(pProtocol);
	return pDbg;
}


bool NetworkManager::mLockProxy = false;

NetworkManager::NetworkManager()
	: QObject()
	, mApplicationExitInProgress(false)
	, mOpenConnectionCount(0)
	, mNetAccessManager(new QNetworkAccessManager())
	, mNetAccessManagerPsk(new QNetworkAccessManager())
{
	if (Instance.exists())
	{
		mNetAccessManager->useAuthenticationManagerFrom(*Instance->mNetAccessManager);
		mNetAccessManagerPsk->useAuthenticationManagerFrom(*Instance->mNetAccessManagerPsk);

		connect(&NetworkManager::getGlobalInstance(), &NetworkManager::fireShutdown, this, &NetworkManager::onShutdown);
	}
	else
	{
		mNetAccessManagerPsk->useAuthenticationManagerFrom(*mNetAccessManager);
	}

#ifndef QT_NO_NETWORKPROXY
	connect(mNetAccessManager.data(), &QNetworkAccessManager::proxyAuthenticationRequired, this, &NetworkManager::fireProxyAuthenticationRequired);
	connect(mNetAccessManagerPsk.data(), &QNetworkAccessManager::proxyAuthenticationRequired, this, &NetworkManager::fireProxyAuthenticationRequired);
#endif
}


NetworkManager::~NetworkManager()
{
}


NetworkManager& NetworkManager::getGlobalInstance()
{
	return *Instance;
}


QNetworkReply* NetworkManager::paos(QNetworkRequest& pRequest, const QByteArray& pData, bool pUsePsk, int pTimeoutInMilliSeconds)
{
	if (mApplicationExitInProgress)
	{
		return new NetworkReplyError(pRequest);
	}

	pRequest.setHeader(QNetworkRequest::UserAgentHeader, getUserAgentHeader());
	pRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/vnd.paos+xml; charset=UTF-8");
	pRequest.setHeader(QNetworkRequest::ContentLengthHeader, QString::number(pData.size()));
	pRequest.setRawHeader("Connection", "keep-alive");
	pRequest.setRawHeader("Accept", "text/html; application/vnd.paos+xml");
	pRequest.setRawHeader("PAOS", "ver=\"urn:liberty:paos:2006-08\"");

	QNetworkReply* response;
	if (pUsePsk)
	{
		pRequest.setSslConfiguration(AppSettings::getInstance().getSecureStorage().getTlsSettingsPsk().getConfiguration());
		response = mNetAccessManagerPsk->post(pRequest, pData);
	}
	else
	{
		pRequest.setSslConfiguration(AppSettings::getInstance().getSecureStorage().getTlsSettings().getConfiguration());
		response = mNetAccessManager->post(pRequest, pData);
	}

	trackConnection(response, pTimeoutInMilliSeconds);
	return response;
}


QNetworkReply* NetworkManager::get(QNetworkRequest& pRequest, int pTimeoutInMilliSeconds)
{
	if (mApplicationExitInProgress)
	{
		return new NetworkReplyError(pRequest);
	}

	pRequest.setHeader(QNetworkRequest::UserAgentHeader, getUserAgentHeader());
	pRequest.setSslConfiguration(AppSettings::getInstance().getSecureStorage().getTlsSettings().getConfiguration());
	QNetworkReply* response = mNetAccessManager->get(pRequest);
	trackConnection(response, pTimeoutInMilliSeconds);
	return response;
}


QSslSocket* NetworkManager::createSslSocket(const QUrl& pUrl)
{
	QSslSocket* socket = new QSslSocket();
	socket->setSslConfiguration(AppSettings::getInstance().getSecureStorage().getTlsSettings().getConfiguration());
	configureProxy(socket, pUrl);
	return socket;
}


void NetworkManager::configureProxy(QTcpSocket* pSocket, const QUrl& pUrl)
{
#ifndef QT_NO_NETWORKPROXY
	// set the proxy explicitly, otherwise the proxy exception don't count
	QNetworkProxyQuery query(pUrl, QNetworkProxyQuery::QueryType::TcpSocket);
	const QNetworkProxy& proxy = QNetworkProxyFactory::proxyForQuery(query).constLast();
	pSocket->setProxy(proxy);

	connect(pSocket, &QTcpSocket::proxyAuthenticationRequired, this, &NetworkManager::fireProxyAuthenticationRequired);
#else
	Q_UNUSED(pSocket)
	Q_UNUSED(pUrl)
#endif
}


QString NetworkManager::getUserAgentHeader() const
{
	const auto& info = VersionInfo::getInstance();
	const QString spec = QStringLiteral(" (%1/%2)").arg(info.getSpecificationTitle(), info.getSpecificationVersion());
	return QCoreApplication::applicationName() % QLatin1Char('/') % QCoreApplication::applicationVersion() % spec;
}


void NetworkManager::onShutdown()
{
	mApplicationExitInProgress = true;
	Q_EMIT fireShutdown();
}


NetworkManager::NetworkError NetworkManager::toNetworkError(const QNetworkReply* const pNetworkReply)
{
	qCDebug(network) << "Select error message for:" << pNetworkReply->error();
	switch (pNetworkReply->error())
	{
		case QNetworkReply::TimeoutError:
			return NetworkError::TimeOut;

		case QNetworkReply::ProxyAuthenticationRequiredError:
		case QNetworkReply::ProxyConnectionClosedError:
		case QNetworkReply::ProxyConnectionRefusedError:
		case QNetworkReply::ProxyNotFoundError:
		case QNetworkReply::ProxyTimeoutError:
		case QNetworkReply::UnknownProxyError:
			return NetworkError::ProxyError;

		case QNetworkReply::SslHandshakeFailedError:
			return NetworkError::SslError;

		default:
			qCCritical(network) << "Network error opening URL" << pNetworkReply->request().url().toString();
			qCCritical(network) << "Network error description" << pNetworkReply->errorString();
			return NetworkError::OtherError;
	}
}


GlobalStatus NetworkManager::toTrustedChannelStatus(const QNetworkReply* const pNetworkReply)
{
	switch (toNetworkError(pNetworkReply))
	{
		case NetworkManager::NetworkError::TimeOut:
			return GlobalStatus::Code::Workflow_TrustedChannel_TimeOut;

		case NetworkManager::NetworkError::ProxyError:
			return GlobalStatus::Code::Workflow_TrustedChannel_Proxy_Error;

		case NetworkManager::NetworkError::SslError:
			return GlobalStatus::Code::Workflow_TrustedChannel_Ssl_Establishment_Error;

		case NetworkManager::NetworkError::OtherError:
			return GlobalStatus::Code::Workflow_TrustedChannel_Other_Network_Error;
	}

	return GlobalStatus::Code::Unknown_Error;
}


GlobalStatus NetworkManager::toStatus(const QNetworkReply* const pNetworkReply)
{
	switch (toNetworkError(pNetworkReply))
	{
		case NetworkManager::NetworkError::TimeOut:
			return GlobalStatus::Code::Network_TimeOut;

		case NetworkManager::NetworkError::ProxyError:
			return GlobalStatus::Code::Network_Proxy_Error;

		case NetworkManager::NetworkError::SslError:
			return GlobalStatus::Code::Network_Ssl_Establishment_Error;

		case NetworkManager::NetworkError::OtherError:
			return GlobalStatus::Code::Network_Other_Error;
	}

	return GlobalStatus::Code::Unknown_Error;
}


void NetworkManager::trackConnection(QNetworkReply* pResponse, const int pTimeoutInMilliSeconds)
{
	Q_ASSERT(pResponse);

	mOpenConnectionCount++;
	connect(pResponse, &QNetworkReply::finished, [&] {
				--mOpenConnectionCount;
			});

	NetworkReplyTimeout::setTimeout(pResponse, pTimeoutInMilliSeconds);
}


#ifndef QT_NO_NETWORKPROXY
namespace
{
class NoProxyFactory
	: public QNetworkProxyFactory
{
	private:
		QList<QNetworkProxy> mProxies;

	public:
		NoProxyFactory()
			: QNetworkProxyFactory()
			, mProxies()
		{
			mProxies << QNetworkProxy::NoProxy;
		}


		virtual QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery& pInputQuery = QNetworkProxyQuery()) override
		{
			qCDebug(network) << pInputQuery;
			qCDebug(network) << "Found proxies" << mProxies;
			return mProxies;
		}


};


class SystemProxyFactory
	: public QNetworkProxyFactory
{
	public:
		SystemProxyFactory()
			: QNetworkProxyFactory()
		{
		}


		virtual QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery& pInputQuery = QNetworkProxyQuery()) override
		{
			qCDebug(network) << pInputQuery;
			QList<QNetworkProxy> proxies = systemProxyForQuery(pInputQuery);
			qCDebug(network) << "Found proxies" << proxies;
			return proxies;
		}


};

}

void NetworkManager::setApplicationProxyFactory()
{
	if (Q_UNLIKELY(mLockProxy))
	{
		qCWarning(network) << "Won't use system proxy because user locked it!";
		qCDebug(network) << "proxy -> none";
		QNetworkProxyFactory::setApplicationProxyFactory(new NoProxyFactory());
	}
	else
	{
		qCDebug(network) << "proxy -> system";
		QNetworkProxyFactory::setApplicationProxyFactory(new SystemProxyFactory());
	}
}


#endif

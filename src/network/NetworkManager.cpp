/*
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "NetworkManager.h"

#include "ErrorMessage.h"
#include "NetworkReplyError.h"
#include "NetworkReplyTimeout.h"
#include "TlsConfiguration.h"
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

	connect(mNetAccessManager.data(), &QNetworkAccessManager::proxyAuthenticationRequired, this, &NetworkManager::fireProxyAuthenticationRequired);
	connect(mNetAccessManagerPsk.data(), &QNetworkAccessManager::proxyAuthenticationRequired, this, &NetworkManager::fireProxyAuthenticationRequired);
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
		pRequest.setSslConfiguration(TlsConfiguration::createPskSslConfiguration());
		response = mNetAccessManagerPsk->post(pRequest, pData);
	}
	else
	{
		pRequest.setSslConfiguration(TlsConfiguration::createSslConfiguration());
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
	pRequest.setSslConfiguration(TlsConfiguration::createSslConfiguration());
	QNetworkReply* response = mNetAccessManager->get(pRequest);
	trackConnection(response, pTimeoutInMilliSeconds);
	return response;
}


QSslSocket* NetworkManager::createSslSocket(const QUrl& pUrl)
{
	QSslSocket* socket = new QSslSocket();
	socket->setSslConfiguration(TlsConfiguration::createSslConfiguration());
	configureProxy(socket, pUrl);
	return socket;
}


void NetworkManager::configureProxy(QTcpSocket* pSocket, const QUrl& pUrl)
{
	// set the proxy explicitly, otherwise the proxy exception don't count
	QNetworkProxyQuery query(pUrl, QNetworkProxyQuery::QueryType::TcpSocket);
	QNetworkProxy proxy = QNetworkProxyFactory::proxyForQuery(query).last();
	pSocket->setProxy(proxy);

	connect(pSocket, &QTcpSocket::proxyAuthenticationRequired, this, &NetworkManager::fireProxyAuthenticationRequired);
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


QString NetworkManager::getLocalizedErrorString(QNetworkReply* pNetworkReply)
{
	qCDebug(network) << "Select error message for:" << pNetworkReply->error();
	const QString requestUrl = pNetworkReply->request().url().toString();
	switch (pNetworkReply->error())
	{
		case QNetworkReply::TimeoutError:
			return ErrorMessage::toString(ErrorMessageId::TIME_OUT);

		// Fall through
		case QNetworkReply::ProxyAuthenticationRequiredError:
		case QNetworkReply::ProxyConnectionClosedError:
		case QNetworkReply::ProxyConnectionRefusedError:
		case QNetworkReply::ProxyNotFoundError:
		case QNetworkReply::ProxyTimeoutError:
		case QNetworkReply::UnknownProxyError:
			return ErrorMessage::toString(ErrorMessageId::PROXY_ERROR);

		case QNetworkReply::SslHandshakeFailedError:
			return ErrorMessage::toString(ErrorMessageId::SSL_ERROR);

		default:
			return getLocalizedErrorString(pNetworkReply->errorString(), requestUrl);
	}
}


QString NetworkManager::getLocalizedErrorString(QTcpSocket* pTcpSocket, const QString& pPeerAddress)
{
	return getLocalizedErrorString(pTcpSocket->errorString(), pPeerAddress);
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


QString NetworkManager::getLocalizedErrorString(const QString& pErrorString, const QString& pRequestUrl)
{
	qCCritical(network) << "Network error opening URL" << pRequestUrl;
	qCCritical(network) << "Network error description" << pErrorString;
	return ErrorMessage::tr(ErrorMessage::CONTACT_SUPPORT);
}


QDebug operator <<(QDebug pDbg, const QNetworkProxyQuery& pQuery)
{
	QDebugStateSaver saver(pDbg);
	pDbg.nospace() << "ProxyQuery(type:" << (int) pQuery.queryType() << ", protocol:" << pQuery.protocolTag() << ", peerPort:" << pQuery.peerPort() << ", peerHostName:" << pQuery.peerHostName() << ", localPort:" << pQuery.localPort() << ", url:" << pQuery.url() << ")";
	return pDbg;
}


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

void NetworkManager::setProxy(QNetworkProxy::ProxyType pProxyType)
{
	if (Q_UNLIKELY(mLockProxy))
	{
		qCWarning(network) << "Won't use proxy settings because they are locked!";
		pProxyType = QNetworkProxy::NoProxy;
	}

	switch (pProxyType)
	{
		case QNetworkProxy::NoProxy:
			qCDebug(network) << "proxy -> none";
			QNetworkProxyFactory::setApplicationProxyFactory(new NoProxyFactory());
			break;

		default:
		case QNetworkProxy::DefaultProxy:
			qCDebug(network) << "proxy -> system";
			QNetworkProxyFactory::setApplicationProxyFactory(new SystemProxyFactory());
			break;
	}
}

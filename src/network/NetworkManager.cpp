/*
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "NetworkManager.h"

#include "NetworkReplyError.h"
#include "NetworkReplyTimeout.h"
#include "SecureStorage.h"
#include "SingletonHelper.h"
#include "VersionInfo.h"

#include <http_parser.h>
#include <QCoreApplication>
#include <QLoggingCategory>
#include <QNetworkProxyFactory>
#include <QRegularExpression>
#include <QStringBuilder>


using namespace governikus;

defineSingleton(NetworkManager)

Q_DECLARE_LOGGING_CATEGORY(network)

bool NetworkManager::mLockProxy = false;

NetworkManager::NetworkManager()
	: QObject()
	, mApplicationExitInProgress(false)
	, mTrackedConnectionsMutex()
	, mTrackedConnections()
	, mNetAccessManager(new QNetworkAccessManager())
{
#ifndef QT_NO_NETWORKPROXY
	connect(mNetAccessManager.data(), &QNetworkAccessManager::proxyAuthenticationRequired, this, &NetworkManager::fireProxyAuthenticationRequired);
#endif
}


NetworkManager::~NetworkManager()
{
}


NetworkManager& NetworkManager::getInstance()
{
	return *Instance;
}


int NetworkManager::getOpenConnectionCount()
{
	QMutexLocker locker(&mTrackedConnectionsMutex);

	return mTrackedConnections.size();
}


void NetworkManager::clearConnections()
{
	mNetAccessManager->clearConnectionCache();
}


QNetworkReply* NetworkManager::paos(QNetworkRequest& pRequest,
		const QByteArray& pNamespace,
		const QByteArray& pData,
		bool pUsePsk,
		const QByteArray& pSslSession,
		int pTimeoutInMilliSeconds)
{
	if (mApplicationExitInProgress)
	{
		return new NetworkReplyError(pRequest);
	}

	pRequest.setHeader(QNetworkRequest::UserAgentHeader, getUserAgentHeader());
	pRequest.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/vnd.paos+xml; charset=UTF-8"));
	pRequest.setHeader(QNetworkRequest::ContentLengthHeader, QString::number(pData.size()));
	pRequest.setRawHeader("Connection", "keep-alive");
	pRequest.setRawHeader("Accept", "text/html; application/vnd.paos+xml");
	pRequest.setRawHeader("PAOS", QByteArray("ver=\"%1\"").replace(QByteArray("%1"), pNamespace));

	QNetworkReply* response;
	SecureStorage::TlsSuite tlsSuite = pUsePsk ? SecureStorage::TlsSuite::PSK : SecureStorage::TlsSuite::DEFAULT;
	auto cfg = SecureStorage::getInstance().getTlsConfig(tlsSuite).getConfiguration();
	cfg.setSessionTicket(pSslSession);
	pRequest.setSslConfiguration(cfg);
	response = mNetAccessManager->post(pRequest, pData);

	trackConnection(response, pTimeoutInMilliSeconds);
	return response;
}


QNetworkReply* NetworkManager::get(QNetworkRequest& pRequest,
		const QByteArray& pSslSession,
		int pTimeoutInMilliSeconds)
{
	if (mApplicationExitInProgress)
	{
		return new NetworkReplyError(pRequest);
	}

	pRequest.setHeader(QNetworkRequest::UserAgentHeader, getUserAgentHeader());
	auto cfg = SecureStorage::getInstance().getTlsConfig().getConfiguration();
	cfg.setSessionTicket(pSslSession);
	pRequest.setSslConfiguration(cfg);
	QNetworkReply* response = mNetAccessManager->get(pRequest);
	trackConnection(response, pTimeoutInMilliSeconds);
	return response;
}


QNetworkReply* NetworkManager::post(QNetworkRequest& pRequest,
		const QByteArray& pData,
		int pTimeoutInMilliSeconds)
{
	if (mApplicationExitInProgress)
	{
		return new NetworkReplyError(pRequest);
	}

	pRequest.setHeader(QNetworkRequest::UserAgentHeader, getUserAgentHeader());
	pRequest.setHeader(QNetworkRequest::ContentLengthHeader, QString::number(pData.size()));

	auto cfg = SecureStorage::getInstance().getTlsConfig(SecureStorage::TlsSuite::DEFAULT).getConfiguration();
	pRequest.setSslConfiguration(cfg);
	QNetworkReply* response = mNetAccessManager->post(pRequest, pData);

	trackConnection(response, pTimeoutInMilliSeconds);
	return response;
}


bool NetworkManager::checkUpdateServerCertificate(const QNetworkReply& pReply)
{
	const QVector<QSslCertificate>& trustedCertificates = SecureStorage::getInstance().getUpdateCertificates();

	const auto& cert = pReply.sslConfiguration().peerCertificate();
	return !cert.isNull() && trustedCertificates.contains(cert);
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

		case QNetworkReply::ServiceUnavailableError:
			return NetworkError::ServiceUnavailable;

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
		case NetworkManager::NetworkError::ServiceUnavailable:
			return GlobalStatus::Code::Workflow_TrustedChannel_ServiceUnavailable;

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
		case NetworkManager::NetworkError::ServiceUnavailable:
			return GlobalStatus::Code::Network_ServiceUnavailable;

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

	addTrackedConnection(pResponse);

	connect(pResponse, &QObject::destroyed, this, [ = ] {
				this->removeTrackedConnection(pResponse);
			});

	NetworkReplyTimeout::setTimeout(pResponse, pTimeoutInMilliSeconds);
}


void NetworkManager::addTrackedConnection(QNetworkReply* pResponse)
{
	QMutexLocker locker(&mTrackedConnectionsMutex);

	mTrackedConnections.insert(pResponse);
}


void NetworkManager::removeTrackedConnection(QNetworkReply* pResponse)
{
	QMutexLocker locker(&mTrackedConnectionsMutex);

	mTrackedConnections.remove(pResponse);
}


QByteArray NetworkManager::getStatusMessage(int pStatus)
{
	switch (pStatus)
	{
	#define XX(num, name, string) case num:\
		return QByteArrayLiteral(#string);

		HTTP_STATUS_MAP(XX)
	#undef XX
	}

	return QByteArray();
}


int NetworkManager::getLoggedStatusCode(const QNetworkReply* const pReply, const QMessageLogger& pLogger)
{
	const int statusCode = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	pLogger.debug() << "Status Code:" << statusCode << getStatusMessage(statusCode);
	for (const auto& header : pReply->rawHeaderPairs())
	{
		pLogger.debug().nospace().noquote() << "Header | " << header.first << ": " << header.second;
	}
	return statusCode;
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

} // namespace

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

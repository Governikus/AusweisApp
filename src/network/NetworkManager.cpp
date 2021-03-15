/*
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#include "NetworkManager.h"

#include "AppSettings.h"
#include "NetworkReplyError.h"
#include "NetworkReplyTimeout.h"
#include "SecureStorage.h"
#include "VersionInfo.h"

#include <http_parser.h>
#include <QCoreApplication>
#include <QLoggingCategory>
#include <QNetworkProxyFactory>
#include <QStringBuilder>


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(network)

bool NetworkManager::mLockProxy = false;

NetworkManager::NetworkManager()
	: QObject()
	, mNetAccessManager()
	, mApplicationExitInProgress(false)
	, mOpenConnectionCount(0)
{
	connect(&mNetAccessManager, &QNetworkAccessManager::proxyAuthenticationRequired, this, &NetworkManager::fireProxyAuthenticationRequired);

	connect(&Env::getSingleton<AppSettings>()->getGeneralSettings(), &GeneralSettings::fireProxyChanged, this, &NetworkManager::onProxyChanged);
}


NetworkManager::~NetworkManager()
{
}


int NetworkManager::getOpenConnectionCount()
{
	return mOpenConnectionCount;
}


void NetworkManager::clearConnections()
{
	mNetAccessManager.clearConnectionCache();
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

	SecureStorage::TlsSuite tlsSuite = pUsePsk ? SecureStorage::TlsSuite::PSK : SecureStorage::TlsSuite::DEFAULT;
	auto cfg = Env::getSingleton<SecureStorage>()->getTlsConfig(tlsSuite).getConfiguration();
	cfg.setSessionTicket(pSslSession);
	pRequest.setSslConfiguration(cfg);

	QNetworkReply* response = mNetAccessManager.post(pRequest, pData);
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
	auto cfg = Env::getSingleton<SecureStorage>()->getTlsConfig().getConfiguration();
	cfg.setSessionTicket(pSslSession);
	pRequest.setSslConfiguration(cfg);

	QNetworkReply* response = mNetAccessManager.get(pRequest);
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

	auto cfg = Env::getSingleton<SecureStorage>()->getTlsConfig(SecureStorage::TlsSuite::DEFAULT).getConfiguration();
	pRequest.setSslConfiguration(cfg);

	QNetworkReply* response = mNetAccessManager.post(pRequest, pData);
	trackConnection(response, pTimeoutInMilliSeconds);
	return response;
}


bool NetworkManager::checkUpdateServerCertificate(const QSharedPointer<const QNetworkReply>& pReply)
{
	const QVector<QSslCertificate>& trustedCertificates = Env::getSingleton<SecureStorage>()->getUpdateCertificates();

	const auto& cert = pReply->sslConfiguration().peerCertificate();
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
	mNetAccessManager.clearAccessCache();
	mNetAccessManager.clearConnectionCache();
	Q_EMIT fireShutdown();
}


void NetworkManager::onProxyChanged()
{
	setApplicationProxyFactory();
}


NetworkManager::NetworkError NetworkManager::toNetworkError(const QSharedPointer<const QNetworkReply>& pNetworkReply)
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
			return NetworkError::SecurityError;

		default:
			qCCritical(network) << "Network error opening URL" << pNetworkReply->request().url().toString();
			qCCritical(network) << "Network error description" << pNetworkReply->errorString();
			return NetworkError::OtherError;
	}
}


GlobalStatus NetworkManager::toTrustedChannelStatus(const QSharedPointer<const QNetworkReply>& pNetworkReply)
{
	const GlobalStatus::ExternalInfoMap infoMap {
		{GlobalStatus::ExternalInformation::LAST_URL, pNetworkReply->url().toString()}
	};
	switch (toNetworkError(pNetworkReply))
	{
		case NetworkManager::NetworkError::ServiceUnavailable:
			return {GlobalStatus::Code::Workflow_TrustedChannel_ServiceUnavailable, infoMap};

		case NetworkManager::NetworkError::TimeOut:
			return {GlobalStatus::Code::Workflow_TrustedChannel_TimeOut, infoMap};

		case NetworkManager::NetworkError::ProxyError:
			return {GlobalStatus::Code::Workflow_TrustedChannel_Proxy_Error, infoMap};

		case NetworkManager::NetworkError::SecurityError:
			return {GlobalStatus::Code::Workflow_TrustedChannel_Establishment_Error, infoMap};

		case NetworkManager::NetworkError::OtherError:
			return {GlobalStatus::Code::Workflow_TrustedChannel_Other_Network_Error, infoMap};
	}

	return {GlobalStatus::Code::Unknown_Error, infoMap};
}


GlobalStatus NetworkManager::toStatus(const QSharedPointer<const QNetworkReply>& pNetworkReply)
{
	const GlobalStatus::ExternalInfoMap infoMap {
		{GlobalStatus::ExternalInformation::LAST_URL, pNetworkReply->url().toString()}
	};
	switch (toNetworkError(pNetworkReply))
	{
		case NetworkManager::NetworkError::ServiceUnavailable:
			return {GlobalStatus::Code::Network_ServiceUnavailable, infoMap};

		case NetworkManager::NetworkError::TimeOut:
			return {GlobalStatus::Code::Network_TimeOut, infoMap};

		case NetworkManager::NetworkError::ProxyError:
			return {GlobalStatus::Code::Network_Proxy_Error, infoMap};

		case NetworkManager::NetworkError::SecurityError:
			return {GlobalStatus::Code::Network_Ssl_Establishment_Error, infoMap};

		case NetworkManager::NetworkError::OtherError:
			return {GlobalStatus::Code::Network_Other_Error, infoMap};
	}

	return {GlobalStatus::Code::Unknown_Error, infoMap};
}


void NetworkManager::trackConnection(QNetworkReply* pResponse, const int pTimeoutInMilliSeconds)
{
	Q_ASSERT(pResponse);

	if (pResponse)
	{
		++mOpenConnectionCount;

		connect(pResponse, &QObject::destroyed, this, [this] {
					--mOpenConnectionCount;
				});

		NetworkReplyTimeout::setTimeout(pResponse, pTimeoutInMilliSeconds);
	}
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


int NetworkManager::getLoggedStatusCode(const QSharedPointer<const QNetworkReply>& pReply, const QMessageLogger& pLogger)
{
	const int statusCode = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	pLogger.debug() << "Status Code:" << statusCode << getStatusMessage(statusCode);
	for (const auto& header : pReply->rawHeaderPairs())
	{
		pLogger.debug().nospace().noquote() << "Header | " << header.first << ": " << header.second;
	}
	return statusCode;
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


		QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery& pInputQuery = QNetworkProxyQuery()) override
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


		QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery& pInputQuery = QNetworkProxyQuery()) override
		{
			qCDebug(network) << pInputQuery;
			QList<QNetworkProxy> proxies = systemProxyForQuery(pInputQuery);
			qCDebug(network) << "Found proxies" << proxies;
			return proxies;
		}


};

class CustomProxyFactory
	: public QNetworkProxyFactory
{
	public:
		CustomProxyFactory()
			: QNetworkProxyFactory()
		{
		}


		QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery& pInputQuery = QNetworkProxyQuery()) override
		{
			qCDebug(network) << pInputQuery;
			const auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			auto proxy = QNetworkProxy(settings.getCustomProxyType(), settings.getCustomProxyHost(), settings.getCustomProxyPort());
			auto proxies = QList<QNetworkProxy>({proxy});
			qCDebug(network) << "Found proxies" << proxies;
			return proxies;
		}


};

} // namespace

void NetworkManager::setApplicationProxyFactory()
{
	const auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	if (Q_UNLIKELY(mLockProxy))
	{
		qCWarning(network) << "Won't use any proxy because user locked it!";
		qCDebug(network) << "proxy -> none";
		QNetworkProxyFactory::setApplicationProxyFactory(new NoProxyFactory());
	}
	else if (settings.useCustomProxy())
	{
		qCDebug(network) << "proxy -> custom";
		QNetworkProxyFactory::setApplicationProxyFactory(new CustomProxyFactory());
	}
	else
	{
		qCDebug(network) << "proxy -> system";
		QNetworkProxyFactory::setApplicationProxyFactory(new SystemProxyFactory());
	}
}
